#ifndef SPARSEMATRIX_HPP
#define SPARSEMATRIX_HPP

#include <vector>

#include "../lyra_common.hpp"

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
    SparseMatrix()
    {
    }

    /**
     * @brief Destroy the Sparse Matrix object
     * 
     */
    ~SparseMatrix()
    {
    }

    /**
     * @brief Product Matrix vector
     * 
     * @param vec std::vector<T>
     * @return std::vector<T> 
     */
    LYRA_HARD_INLINE
    std::vector<T>
    operator*(const std::vector<T> &vec) const;

protected:
    /* data */

private:
    SparseMatrix(const SparseMatrix &) = delete;
};

#endif // SPARSEMATRIX_HPP