#include "mesh.hpp"

#include <iomanip>
#include <iostream>
#include <tuple>
#include <utility>

#include "point.hpp"

Mesh::Mesh () : m_numCells (0x0), m_points ({}), m_receiver_points ({}), m_sender_points ({}), m_physical_points ({}) {}

Mesh::~Mesh ()
{
    for (Point *p : m_points)
        delete p;
    m_points.clear ();

    m_receiver_points.clear ();
    m_sender_points.clear ();
    m_physical_points.clear ();
}

void
Mesh::InitAndReserve (ul_t indicPoint, ul_t numProcs)
{
    // Clear
    for (Point *p : m_points)
        delete p;
    m_points.clear ();

    m_numCells = 0x0;
    m_receiver_points.clear ();
    m_sender_points.clear ();
    m_physical_points.clear ();

    //    for (ul_t id = 0; id < numProcs - 1; ++id)
    //    {
    //        m_sender_points.push_back (std::vector<Point *> (0));
    //        m_receiver_points.push_back (std::vector<Point *> (0));
    //    }

    m_sender_points.resize (numProcs);
    m_receiver_points.resize (numProcs);

    m_points.reserve (indicPoint);
    m_physical_points.reserve (indicPoint * 30 / 100);
    return;
}

void
Mesh::operator+= (Point *p)
{
    p->localId = m_points.size ();
    m_points.push_back (p);

    if (p->tag & PT_SEND)
        for (ul_t idProc : p->procsidx)
            m_sender_points [idProc].push_back (p);

    if (p->tag & PT_RECEIVE)
        for (ul_t idProc : p->procsidx)
            m_receiver_points [idProc].push_back (p);

    if (p->tag & PT_PHYSICAL)
        m_physical_points.push_back (p);

    return;
}

ul_t
Mesh::GetNumberOfEdges () const
{
    ul_t counter = 0;

    for (Point *p : m_points)
        counter += p->NumberOfNeighs ();

    return counter;
}

void
Mesh::Finalize ()
{
    auto comparator = [] (Point *a, Point *b) -> bool { return (a->globalId <= b->globalId); };

    for (ul_t count = 0; count < m_sender_points.size (); ++count)
        std::sort (m_sender_points [count].begin (), m_sender_points [count].end (), comparator);

    for (ul_t count = 0; count < m_receiver_points.size (); ++count)
        std::sort (m_receiver_points [count].begin (), m_receiver_points [count].end (), comparator);

    std::sort (m_physical_points.begin (), m_physical_points.end (), comparator);

    return;
}

void
Mesh::PrintCommunications (ul_t proc)
{
    ul_t numProcs = m_sender_points.size ();

    std::cout << proc << " |" << std::setw (10) << "SEND"
              << " | " << std::setw (10) << "RECEIVE"
              << " |\n";

    std::cout << std::string (28, '*') << "\n";

    for (ul_t idProc = 0; idProc < numProcs; ++idProc)
        std::cout << idProc << " |" << std::setw (10) << m_sender_points [idProc].size () << " | " << std::setw (10)
                  << m_receiver_points [idProc].size () << " |"
                  << "\n";

    std::cout << std::string (28, '*') << "\n";
    std::cout << "  |" << std::setw (10) << GetTotalNumberOfSenderPoints () << " | " << std::setw (10)
              << GetTotalNumberOfReceiverPoints () << " |"
              << "\n";

    std::cout << std::endl;
    return;
}

std::ostream &
operator<< (std::ostream &flux, const Mesh &mesh)
{
    flux << "[" << mesh.m_points.size () << " npts, " << mesh.m_numCells << " ncells]" << std::flush;

    return flux;
}
