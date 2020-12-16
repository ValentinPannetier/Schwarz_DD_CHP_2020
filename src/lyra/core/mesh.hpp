#ifndef SRC_LYRA_CORE_MESH_HPP
#define SRC_LYRA_CORE_MESH_HPP

#include <iostream>
#include <vector>

#include "common.hpp"
#include "enumlist.hpp"

class Point;

class Mesh
{
public:
    Mesh ();

    ~Mesh ();

    void
    InitAndReserve (ul_t indic);

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

    void
    operator+= (Point * p);

    LYRA_INLINE
    Point *
    GetPoint (ul_t id)
    {
        return m_points [id];
    }

    LYRA_INLINE
    ul_t
    GetNumberOfPoints () const
    {
        return m_points.size ();
    }

    ul_t
    GetNumberOfEdges () const;

    LYRA_INLINE
    std::vector<ul_t> *
    GetListOfPointsWithTag (PTAG tag)
    {
        return &m_tags_points [tag];
    }

    friend std::ostream &
    operator<< (std::ostream & flux, const Mesh & mesh);

protected:
    ul_t                            m_numCells;
    std::vector<Point *>            m_points;
    std::vector<Point *>            m_shared;
    std::vector<std::vector<ul_t> > m_tags_points;

private:
    Mesh (const Mesh &) = delete;
};

std::ostream &
operator<< (std::ostream & flux, const Mesh & mesh);

#endif /* SRC_LYRA_CORE_MESH_HPP */
