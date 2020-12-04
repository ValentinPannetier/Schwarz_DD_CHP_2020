#ifndef SPARSESOLVER_HPP
#define SPARSESOLVER_HPP

#include "../Data/enumlist.hpp"
#include "../SparseMatrix/sparsematrix.hpp"
#include "../lyra_common.hpp"

#define DOT(A, B) std::inner_product (A.begin (), A.end (), B.begin (), 0.0)

template <typename T>
class SparseSolverBase
{
public:
    explicit SparseSolverBase (const SparseMatrix<T> & matrix) : m_matrix (matrix)
    {
    }

    ~SparseSolverBase () {}

    virtual std::vector<T>
    Solve (const std::vector<T> & b) = 0;

protected:
    const SparseMatrix<T> & m_matrix;

private:
    SparseSolverBase (const SparseSolverBase &) = delete;
};

template <typename T>
class SparseCG : public SparseSolverBase<T>
{
public:
    using SparseSolverBase<T>::SparseSolverBase;
    using SuperClass = SparseSolverBase<T>;

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
     * @return std::vector<T> 
     */
    std::vector<T>
    Solve (const std::vector<T> & b) override
    {
        using Type = std::vector<T>;

        T eps = 1e-10;

        ul_t size = SuperClass::m_matrix.Size ();

        Type x (size, 0.);

        Type r = b;
        for (ul_t rowid = 0; rowid < size; ++rowid)
            r [rowid] -= SuperClass::m_matrix.RowProduct (x, rowid);

        T rrk   = DOT (r, r);
        T rrk1  = 0.0;
        T alpha = 0.0;
        T beta  = 0.0;

        Type p = r;
        Type Ap (size, 0.0);

        for (ul_t rowid = 0; rowid < size; ++rowid)
            Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

        ul_t iter;
        T    err;
        for (iter = 0; iter < size; ++iter)
        {
            alpha = rrk / DOT (p, Ap);

            for (ul_t rowid = 0; rowid < size; ++rowid)
                x [rowid] += alpha * p [rowid];

            for (ul_t rowid = 0; rowid < size; ++rowid)
                r [rowid] -= alpha * Ap [rowid];

            rrk1 = DOT (r, r);

            err = rrk1;
            if (err < eps)
                break;

            beta = rrk1 / rrk;

            for (ul_t rowid = 0; rowid < size; ++rowid)
                p [rowid] = r [rowid] + beta * p [rowid];

            for (ul_t rowid = 0; rowid < size; ++rowid)
                Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

            rrk = rrk1;
        }

        INFOS << COLOR_BLUE << "Sparse solver : CG " << COLOR_DEFAULT << " [iter = " << iter << " / " << size << ", err = " << err << "/ " << eps << "]" << ENDLINE;

        return x;
    }
};

template <typename T>
class SparseBiCGSTAB : public SparseSolverBase<T>
{
public:
    using SparseSolverBase<T>::SparseSolverBase;
    using SuperClass = SparseSolverBase<T>;

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
     * @return std::vector<T> 
     */
    std::vector<T>
    Solve (const std::vector<T> & b) override
    {
        using Type = std::vector<T>;

        T eps = 1e-20;

        ul_t size = SuperClass::m_matrix.Size ();

        Type x (size, 0.);

        Type r = b;
        for (ul_t rowid = 0; rowid < size; ++rowid)
            r [rowid] -= SuperClass::m_matrix.RowProduct (x, rowid);

        Type r0    = r;
        T    rrk   = DOT (r, r);
        T    rrk1  = 0.0;
        T    alpha = 0.0;
        T    beta  = 0.0;
        T    omega = 0.0;

        Type p = r;
        Type Ap (size, 0.);

        for (ul_t rowid = 0; rowid < size; ++rowid)
            Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

        Type As (size, 0.0);
        Type s (size, 0.0);

        ul_t iter;
        T    err;
        for (iter = 0; iter < size; ++iter)
        {
            alpha = rrk / DOT (Ap, r0);

            for (ul_t rowid = 0; rowid < size; ++rowid)
                s [rowid] = r [rowid] - alpha * Ap [rowid];

            for (ul_t rowid = 0; rowid < size; ++rowid)
                As [rowid] = SuperClass::m_matrix.RowProduct (s, rowid);

            omega = DOT (As, s) / DOT (As, As);

            for (ul_t rowid = 0; rowid < size; ++rowid)
                x [rowid] += alpha * p [rowid] + omega * s [rowid];

            for (ul_t rowid = 0; rowid < size; ++rowid)
                r [rowid] = s [rowid] - omega * As [rowid];

            rrk1 = DOT (r, r0);
            err  = DOT (r, r);
            if (err < eps)
                break;

            beta = (alpha / omega) * (rrk1 / rrk);

            for (ul_t rowid = 0; rowid < size; ++rowid)
                p [rowid] = r [rowid] + beta * (p [rowid] - omega * Ap [rowid]);

            for (ul_t rowid = 0; rowid < size; ++rowid)
                Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

            rrk = rrk1;
        }

        INFOS << COLOR_BLUE << "Sparse solver : BiCGSTAB " << COLOR_DEFAULT << " [iter = " << iter << " / " << size << ", err = " << err << "/ " << eps << "]" << ENDLINE;

        return x;
    }
};
#endif  // SPARSESOLVER_HPP