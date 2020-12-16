#include "mesh.hpp"

#include <tuple>
#include <utility>

#include "point.hpp"

Mesh::Mesh () : m_numCells (0x0),
                m_points ({}),
                m_shared ({}),
                m_tags_points ({})
{
}

Mesh::~Mesh ()
{
    for (Point * p : m_points)
        delete p;
    m_points.clear ();
}

void
Mesh::InitAndReserve (ul_t indic)
{
    // Clear
    for (Point * p : m_points)
        delete p;
    m_points.clear ();

    m_numCells = 0x0;
    m_shared.clear ();
    m_tags_points.clear ();

    // Other part
    m_points.reserve (indic);

    // For all PT_NONE
    m_tags_points.push_back (std::vector<ul_t> ());
    m_tags_points.back ().reserve (indic);

    // For other tags
    for (ul_t tag = PTAG::PT_FIRST + 1; tag < PTAG::PT_LAST; ++tag)
    {
        m_tags_points.push_back (std::vector<ul_t> ());

        // Reserve 10% for each tag
        m_tags_points.back ().reserve (indic / 100 * 10);
    }
}

void
Mesh::operator+= (Point * p)
{
    p->localId = m_points.size ();
    m_points.push_back (p);

    m_tags_points [p->tag].push_back (p->globalId);

    if (p->procsidx.size () != 1)
        m_shared.push_back (p);

    return;
}

ul_t
Mesh::GetNumberOfEdges () const
{
    ul_t counter = 0;
    for (Point * p : m_points)
        counter += p->NumberOfNeighs ();

    return counter;
}

std::ostream &
operator<< (std::ostream & flux, const Mesh & mesh)
{
    flux << "[" << mesh.m_points.size () << " npts, " << mesh.m_numCells << " ncells]" << std::flush;

    return flux;
}