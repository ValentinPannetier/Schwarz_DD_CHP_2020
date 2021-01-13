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
    InitAndReserve (ul_t indicPoint, ul_t numProcs = 0);

    void
    operator+= (Point *p);

    ul_t
    GetNumberOfEdges () const;

    void
    Finalize ();

    void
    PrintCommunications (ul_t proc);

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

    LYRA_INLINE
    std::vector<Point *> *
    GetSenderPointsOnProc (ul_t idproc)
    {
        return &m_sender_points [idproc];
    }

    LYRA_INLINE
    ul_t
    GetNumberOfSenderPointsOnProc (ul_t idproc)
    {
        return m_sender_points [idproc].size ();
    }

    LYRA_INLINE
    ul_t
    GetTotalNumberOfSenderPoints ()
    {
        ul_t out = 0x0;
        for (std::vector<Point *> vec : m_sender_points)
            out += vec.size ();

        return out;
    }

    LYRA_INLINE
    std::vector<Point *> *
    GetReceiverPointsOnProc (ul_t idproc)
    {
        return &m_receiver_points [idproc];
    }

    LYRA_INLINE
    ul_t
    GetNumberOfReceiverPointsOnProc (ul_t idproc)
    {
        return m_receiver_points [idproc].size ();
    }

    LYRA_INLINE
    ul_t
    GetTotalNumberOfReceiverPoints ()
    {
        ul_t out = 0x0;
        for (std::vector<Point *> vec : m_receiver_points)
            out += vec.size ();

        return out;
    }

    LYRA_INLINE
    ul_t
    GetNumberOfPhysicalPoints ()
    {
        return m_physical_points.size ();
    }

    LYRA_INLINE
    std::vector<Point *> *
    GetPhysicalPoints ()
    {
        return &m_physical_points;
    }

    friend std::ostream &
    operator<< (std::ostream &flux, const Mesh &mesh);

    ll_t count_dirichlet = 0;
    ll_t count_neumann   = 0;
    ll_t count_robin     = 0;

protected:
    ul_t                              m_numCells;
    std::vector<Point *>              m_points;
    std::vector<std::vector<Point *>> m_receiver_points;
    std::vector<std::vector<Point *>> m_sender_points;
    std::vector<Point *>              m_physical_points;

    // {idProc, listOfPoints}

private:
    Mesh (const Mesh &) = delete;
};

std::ostream &
operator<< (std::ostream &flux, const Mesh &mesh);

#endif /* SRC_LYRA_CORE_MESH_HPP */
