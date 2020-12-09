#ifndef SRC_LYRA_CORE_MESH_HPP
#define SRC_LYRA_CORE_MESH_HPP

#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "common.hpp"
#include "enumlist.hpp"
#include "point.hpp"

template <typename T>
class Mesh
{
public:
    Mesh () : m_numCells (0x0),
              m_points ({}),
              m_shared ({}),
              m_tags_points ({})
    {
    }

    ~Mesh ()
    {
        for (Point<T> * p : m_points)
            delete p;
        m_points.clear ();
    }

    LYRA_INLINE
    void
    InitAndReserve (ul_t indic)
    {
        // Clear
        for (Point<T> * p : m_points)
            delete p;
        m_points.clear ();

        m_numCells = 0x0;
        m_shared.clear ();
        m_tags_points.clear ();

        // Other part
        m_points.reserve (indic);

        for (ul_t tag = PTAG::PT_FIRST; tag < PTAG::PT_LAST; ++tag)
        {
            m_tags_points.push_back (std::vector<ul_t> ());

            // Reserve 10% for each tag
            m_tags_points.back ().reserve (indic / 100 * 10);
        }
    }

    LYRA_INLINE
    void
    SetNumberOfCells (ul_t numCells)
    {
        m_numCells = numCells;
        return;
    }

    LYRA_INLINE
    ul_t
    GetNumberOfCells ()
    {
        return m_numCells;
    }

    LYRA_INLINE
    void
    operator+= (Point<T> * p)
    {
        p->localId = m_points.size ();
        m_points.push_back (p);
        m_tags_points [p->tag].push_back (p->globalId);

        return;
    }

    LYRA_INLINE
    Point<T> *
    GetPoint (ul_t & id)
    {
        return m_points [id];
    }

    LYRA_INLINE
    ul_t
    GetNumberOfPoints () const
    {
        return m_points.size ();
    }

    LYRA_INLINE
    ul_t
    GetNumberOfEdges () const
    {
        ul_t counter = 0;
        for (Point<T> * p : m_points)
            counter += p->NumberOfNeighs ();

        return counter;
    }

    template <typename U>
    friend std::ostream &
    operator<< (std::ostream & flux, const Mesh<U> & mesh);

protected:
    ul_t                            m_numCells;
    std::vector<Point<T> *>         m_points;
    std::vector<Point<T> *>         m_shared;
    std::vector<std::vector<ul_t> > m_tags_points;

private:
    Mesh (const Mesh &) = delete;
};

template <typename T>
std::ostream &
operator<< (std::ostream & flux, const Mesh<T> & mesh)
{
    flux << "[" << mesh.m_points.size () << " npts, " << mesh.m_numCells << " ncells]" << std::flush;

    return flux;
}

#endif /* SRC_LYRA_CORE_MESH_HPP */
