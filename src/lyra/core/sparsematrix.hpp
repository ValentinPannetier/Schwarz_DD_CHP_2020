#ifndef SRC_LYRA_CORE_SPARSEMATRIX_HPP
#define SRC_LYRA_CORE_SPARSEMATRIX_HPP

#include <iostream>
#include <vector>

#include "common.hpp"

class Triplet
{
public:
    Triplet (ul_t _i, ul_t _j, real_t _value);

    ul_t   i, j;
    real_t value;
};

typedef std::vector<Triplet> TripletsList;

/**
     * @brief Row compact container
     * 
     */
struct RowCompactor
{
    std::vector<real_t> value = {};
    std::vector<ul_t>   colid = {};
};

/**
 * @brief SparseMatrix template class
 * 
 * @tparam T type of data
 */
class SparseMatrix
{
public:
    SparseMatrix ();
    ~SparseMatrix ();

    void
    Init (ul_t numOfPoints);

    void
    Clear ();

    ul_t
    NonZeros ();

    LYRA_HARD_INLINE
    ul_t
    Size () const
    {
        return m_np;
    }

    void
    PrintSparseView (std::ostream & stream = std::cout);

    void
    PrintDenseView (std::ostream & stream = std::cout);

    std::vector<real_t>
    operator* (const std::vector<real_t> & vec) const;

    real_t
    RowProduct (const std::vector<real_t> & vec, ul_t rowid) const;

    void
    SetFromTriplet (typename TripletsList::iterator beg,
                    typename TripletsList::iterator end);

    void
    Pruned (real_t epsilon = 1e-20);

    LYRA_INLINE
    RowCompactor *
    GetRow (ul_t idRow)
    {
        return m_rows [idRow];
    }

protected:
    std::vector<RowCompactor *> m_rows;
    ul_t                        m_np;  // number of points

private:
    SparseMatrix (const SparseMatrix &) = delete;
};

#endif /* SRC_LYRA_CORE_SPARSEMATRIX_HPP */
