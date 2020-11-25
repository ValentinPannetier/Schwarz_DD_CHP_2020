#ifndef SPARSEMATRIX_HPP
#define SPARSEMATRIX_HPP

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "../lyra_common.hpp"
#include "tools.hpp"

template <typename T>
class Triplet
{
public:
    Triplet (ul_t _i, ul_t _j, T _value) : i (_i),
                                           j (_j),
                                           value (_value)
    {
    }

    ul_t i, j;
    T    value;
};

/**
 * @brief SparseMatrix template class
 * 
 * @tparam T type of data
 */
template <typename T>
class SparseMatrix
{
public:
    /**
    * @brief Construct a new Sparse Matrix object
    * 
    */
    SparseMatrix () : m_rows ({}),
                      m_np (0)
    {
    }

    /**
     * @brief Destroy the Sparse Matrix object
     * 
     */
    ~SparseMatrix ()
    {
        Clear ();
    }

    LYRA_HARD_INLINE
    void
    Init (ul_t numOfPoints)
    {
        this->Clear ();
        m_np = numOfPoints;

        for (ul_t id = 0; id < m_np; ++id)
            m_rows.push_back (new RowCompactor ());

        return;
    }

    LYRA_HARD_INLINE
    void
    Clear ()
    {
        m_np = 0;
        for (RowCompactor * row : m_rows)
            delete row;
        m_rows.clear ();
        return;
    }

    LYRA_HARD_INLINE
    ul_t
    NonZeros ()
    {
        ul_t out = 0;
        for (RowCompactor * row : m_rows)
            out += row->value.size ();

        return out;
    }

    void
    PrintSparseView (std::ostream & stream = std::cout)
    {
        RowCompactor * row = nullptr;

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
    PrintDenseView (std::ostream & stream = std::cout)
    {
        RowCompactor * row;
        ul_t           colcursor, colidsize;

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
                    stream << std::setw (8) << row->value [colcursor] << std::flush;
                    colcursor++;
                }
                else
                {
                    stream << std::setw (8) << 0.0 << std::flush;
                }
            }
            stream << std::endl;
        }

        return;
    }

    /**
     * @brief Product Matrix vector
     * 
     * @param vec std::vector<T>
     * @return std::vector<T> 
     */
    LYRA_INLINE
    std::vector<T>
    operator* (const std::vector<T> & vec) const
    {
        std::vector<T> out (vec.size ());

        if (vec.size () != m_np)
            return out;

        RowCompactor * row  = nullptr;
        T *            data = nullptr;
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

    /**
    * @brief Typedef for list of Triplets
    * 
    */
    typedef std::vector<Triplet<T>> TripletsList;

    /**
     * @brief SetFromTriplet function
     * 
     * @param beg 
     * @param end 
     * @return void 
     */
    LYRA_INLINE
    void
    SetFromTriplet (typename TripletsList::iterator beg,
                    typename TripletsList::iterator end)
    {
        bool           find = false;
        RowCompactor * row  = nullptr;

        for (typename TripletsList::iterator it = beg; it < end; ++it)
        {
            if (it->i >= m_np)
                continue;

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

        for (RowCompactor * row : m_rows)
        {
            auto compare = [row] (const ul_t & i, const ul_t & j) -> bool {
                return row->colid [i] < row->colid [j];
            };

            std::vector<ul_t> indices (row->colid.size ());
            std::iota (indices.begin (), indices.end (), 0);

            std::sort (indices.begin (), indices.end (), compare);

            apply_permutation (row->value, indices);
            apply_permutation (row->colid, indices);
        }

        return;
    }

    LYRA_INLINE
    void
    Pruned (T epsilon = 1e-20)
    {
        for (RowCompactor * row : m_rows)
            for (ul_t id = 0; id < row->colid.size (); ++id)
                if (std::abs (row->value [id]) < epsilon)
                {
                    row->value.erase (row->value.begin () + id);
                    row->colid.erase (row->colid.begin () + id);
                }

        return;
    }

protected:
    /**
     * @brief Row compact container
     * 
     */
    struct RowCompactor
    {
        std::vector<T>    value = {};
        std::vector<ul_t> colid = {};
    };

    std::vector<RowCompactor *> m_rows;
    ul_t                        m_np;  // number of points

private:
    SparseMatrix (const SparseMatrix &) = delete;
};

#endif  // SPARSEMATRIX_HPP