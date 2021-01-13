#include "sparsematrix.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>

#include "tools.hpp"

Triplet::Triplet (ul_t _i, ul_t _j, real_t _value) : i (_i), j (_j), value (_value) {}

SparseMatrix::SparseMatrix () : m_rows ({}), m_np (0) {}

SparseMatrix::SparseMatrix (const SparseMatrix &tocopy) : m_rows ({}), m_np (0)
{
    Clear ();
    Init (tocopy.m_np);

    for (ul_t idRow = 0x0; idRow < m_rows.size (); ++idRow)
    {
        m_rows [idRow]->value = tocopy.m_rows [idRow]->value;
        m_rows [idRow]->colid = tocopy.m_rows [idRow]->colid;
    }
}

SparseMatrix::~SparseMatrix ()
{
    m_np = 0;
    for (RowCompactor *row : m_rows)
        delete row;
    m_rows.clear ();
}

void
SparseMatrix::Init (ul_t numOfPoints)
{
    this->Clear ();
    m_np = numOfPoints;

    for (ul_t id = 0; id < m_np; ++id)
        m_rows.push_back (new RowCompactor ());

    return;
}

void
SparseMatrix::Clear ()
{
    m_np = 0;
    for (RowCompactor *row : m_rows)
        delete row;
    m_rows.clear ();
    return;
}

ul_t
SparseMatrix::NonZeros () const
{
    ul_t out = 0;
    for (RowCompactor *row : m_rows)
        out += row->value.size ();

    return out;
}

void
SparseMatrix::PrintSparseView (std::ostream &stream) const
{
    RowCompactor *row = nullptr;

    stream << "NNZ : " << NonZeros () << std::endl;
    for (ul_t rowid = 0; rowid < m_np; ++rowid)
    {
        row = m_rows [rowid];

        stream << "i : " << rowid << std::flush;
        for (ul_t id = 0; id < row->colid.size (); ++id)
            stream << " [j : " << row->colid [id] << ", value : " << row->value [id] << "] \t";
        stream << std::endl;
    }

    return;
}

void
SparseMatrix::PrintDenseView (std::ostream &stream) const
{
    RowCompactor *row;
    ul_t          colcursor, colidsize;

    stream << "NNZ : " << NonZeros () << std::endl;
    for (ul_t rowid = 0; rowid < m_np; ++rowid)
    {
        row       = m_rows [rowid];
        colcursor = 0;
        colidsize = row->colid.size ();

        for (ul_t colid = 0; colid < m_np || colcursor < colidsize; ++colid)
        {
            if ((colcursor < colidsize) && (row->colid [colcursor] == colid))
            {
                stream << std::setw (10) << row->value [colcursor] << std::flush;
                colcursor++;
            }
            else
            {
                stream << std::setw (10) << 0.0 << std::flush;
            }
        }
        stream << std::endl;
    }

    return;
}

std::vector<real_t>
SparseMatrix::operator* (const std::vector<real_t> &vec) const
{
    std::vector<real_t> out (vec.size ());

    if (vec.size () != m_np)
        return out;

    RowCompactor *row  = nullptr;
    real_t *      data = nullptr;
    for (ul_t rowid = 0; rowid < m_np; ++rowid)
    {
        row  = m_rows [rowid];
        data = &out [rowid];

        *data = 0.0;

        for (ul_t id = 0; id < row->colid.size (); ++id)
            *data += row->value [id] * vec [row->colid [id]];
    }

    return out;
}

real_t
SparseMatrix::RowProduct (const std::vector<real_t> &vec, ul_t rowid) const
{
    real_t data = 0;

    ul_t size = m_rows [rowid]->colid.size ();
    for (ul_t id = 0; id < size; ++id)
        data += m_rows [rowid]->value [id] * vec [m_rows [rowid]->colid [id]];

    return data;
}

void
SparseMatrix::SetFromTriplet (typename TripletsList::iterator beg, typename TripletsList::iterator end)
{
    bool          find    = false;
    RowCompactor *row     = nullptr;
    ul_t          notgood = 0x0;

    for (typename TripletsList::iterator it = beg; it < end; ++it)
    {
        if (it->i >= m_np || std::isinf (it->value) || std::isnan (it->value))
        {
            notgood++;
            continue;
        }

        find = false;
        row  = m_rows [it->i];

        for (ul_t id = 0; id < row->colid.size (); ++id)
            if (row->colid [id] == it->j)
            {
                row->value [id] += it->value;
                find = true;
            }

        if (find || it->j >= m_np)
            continue;

        row->colid.push_back (it->j);
        row->value.push_back (it->value);
    }

    for (RowCompactor *row : m_rows)
    {
        auto compare = [row] (const ul_t &i, const ul_t &j) -> bool { return row->colid [i] < row->colid [j]; };

        std::vector<ul_t> indices (row->colid.size ());
        std::iota (indices.begin (), indices.end (), 0);

        std::sort (indices.begin (), indices.end (), compare);

        apply_permutation (row->value, indices);
        apply_permutation (row->colid, indices);
    }

    if (notgood != 0x0)
        WARNING << "You try to set the matric with " << notgood << " not good values or indices... [keep "
                << end - beg - long (notgood) << " entries]" << ENDLINE;

    return;
}

void
SparseMatrix::Pruned (real_t epsilon)
{
    for (RowCompactor *row : m_rows)
    {
        std::vector<ul_t>   ncolid = {};
        std::vector<real_t> nvalue = {};

        ncolid.reserve (row->colid.size ());
        nvalue.reserve (row->value.size ());

        for (ul_t id = 0; id < row->colid.size (); ++id)
            if (std::abs (row->value [id]) > epsilon)
            {
                ncolid.push_back (row->colid [id]);
                nvalue.push_back (row->value [id]);
            }

        row->colid = ncolid;
        row->value = nvalue;
    }

    return;
}
