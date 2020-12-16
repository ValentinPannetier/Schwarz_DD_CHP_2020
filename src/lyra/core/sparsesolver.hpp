#ifndef SRC_LYRA_CORE_SPARSESOLVER_HPP
#define SRC_LYRA_CORE_SPARSESOLVER_HPP

#include <vector>

#include "common.hpp"

#define DOT(A, B) std::inner_product (A.begin (), A.end (), B.begin (), 0.0)

class SparseMatrix;

class SparseSolverBase
{
public:
    explicit SparseSolverBase (const SparseMatrix & matrix);
    ~SparseSolverBase ();

    virtual std::vector<real_t>
    Solve (const std::vector<real_t> & b) = 0;

protected:
    const SparseMatrix & m_matrix;

private:
    SparseSolverBase (const SparseSolverBase &) = delete;
};

class SparseCG : public SparseSolverBase
{
public:
    using SparseSolverBase::SparseSolverBase;
    using SuperClass = SparseSolverBase;

    /**
     * @brief CG
     * 
     * r = b - Ax
     * rrk = <r | r>
     * p = r
     * Ap = A * p
     * 
     * for (k = 0; k < maxiter; ++k)
     * {
     *      a = rrk / <p | Ap>
     *      x += a * p
     *      r -= a * y
     *      
     *      rrk1 = <r | r>
     *      b = rrk1 / rrk
     *      p = r + b * p 
     *      Ap = A * p 
     * }
     * 
     * @param b 
     * @return std::vector<real_t>
     */
    std::vector<real_t>
    Solve (const std::vector<real_t> & b) override;
};

class SparseBiCGSTAB : public SparseSolverBase
{
public:
    using SparseSolverBase::SparseSolverBase;
    using SuperClass = SparseSolverBase;

    /**
     * @brief BiCGSTAB
     * 
     * r = b - Ax
     * r0 = r
     * rrk = <r | r0>
     * p = r
     * Ap = A * p
     * s
     * 
     * for (k = 0; k < maxiter; ++k)
     * {      
     *      alpha = rrk / <Ap | r0>
     *      s = r - alpha * Ap
     *      As = A * s
     *      w = <As | s> / <As | As>
     * 
     *      x += alpha * p + w * s
     *      
     *      r = s - w * As
     *      rrk1 =  <r | r0>
     *      beta = (alpha / w) * rrk1 / rrk
     *      p = r + beta * (p - w * Ap) 
     *      Ap = A * p
     * 
     *      rrk = rrk1
     * }
     * 
     * @param b 
     * @return std::vector 
     */
    std::vector<real_t>
    Solve (const std::vector<real_t> & b) override;
};
#endif /* SRC_LYRA_CORE_SPARSESOLVER_HPP */
