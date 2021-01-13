#ifndef SRC_LYRA_CORE_SPARSESOLVER_HPP
#define SRC_LYRA_CORE_SPARSESOLVER_HPP

#include <string>
#include <vector>

#include "common.hpp"

#define DOT(A, B) std::inner_product (A.begin (), A.end (), B.begin (), 0.0)

class SparseMatrix;

class SparseSolverBase
{
public:
    explicit SparseSolverBase (const SparseMatrix &matrix);
    virtual ~SparseSolverBase ();

    void
    SetTolerance (real_t eps);

    void
    SetMaxIter (ul_t maxiter);

    void
    SetPrefix (std::string prefix);

    virtual void
    Solve (const std::vector<real_t> &b, std::vector<real_t> *solNum) = 0;

protected:
    const SparseMatrix &m_matrix;
    real_t              m_eps;
    std::string         m_prefix;
    ul_t                m_maxiter;

private:
    SparseSolverBase (const SparseSolverBase &) = delete;
};

class SparseCG : public SparseSolverBase
{
public:
    using SuperClass = SparseSolverBase;
    using SuperClass::SuperClass;
    ~SparseCG () override {}

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
     */

    /**
     * @brief Solve
     * @param b
     * @param solNum
     */
    void
    Solve (const std::vector<real_t> &b, std::vector<real_t> *solNum) override;
};

class SparseBiCGSTAB : public SparseSolverBase
{
public:
    using SuperClass = SparseSolverBase;
    using SuperClass::SuperClass;
    ~SparseBiCGSTAB () override {}

    /**
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
     */

    /**
     * @brief Solve
     * @param b
     * @param solNum
     */

    void
    Solve (const std::vector<real_t> &b, std::vector<real_t> *solNum) override;
};
#endif /* SRC_LYRA_CORE_SPARSESOLVER_HPP */
