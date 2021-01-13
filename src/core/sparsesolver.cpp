#include "sparsesolver.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>

#include "sparsematrix.hpp"
#include "tools.hpp"

SparseSolverBase::SparseSolverBase (const SparseMatrix &matrix) :
    m_matrix (matrix),
    m_eps (1e-10),
    m_prefix (""),
    m_maxiter (matrix.Size ())
{
}

SparseSolverBase::~SparseSolverBase () {}

void
SparseSolverBase::SetTolerance (real_t eps)
{
    m_eps = eps;
    return;
}

void
SparseSolverBase::SetMaxIter (ul_t maxiter)
{
    m_maxiter = maxiter;
    return;
}

void
SparseSolverBase::SetPrefix (std::string prefix)
{
    m_prefix = prefix;
    return;
}

void
SparseCG::Solve (const std::vector<real_t> &b, std::vector<real_t> *x)
{
    using Type = std::vector<real_t>;

    ul_t size = SuperClass::m_matrix.Size ();

    if (x->size () != size)
        x->resize (size, 0.0);

    Type r = b;
    for (ul_t rowid = 0; rowid < size; ++rowid)
        r [rowid] -= SuperClass::m_matrix.RowProduct (*x, rowid);

    real_t rrk   = DOT (r, r);
    real_t rrk1  = 0.0;
    real_t alpha = 0.0;
    real_t beta  = 0.0;

    Type p = r;
    Type Ap (size, 0.0);

    for (ul_t rowid = 0; rowid < size; ++rowid)
        Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

    ul_t   iter;
    real_t err = 0.0;

    for (iter = 0; iter < m_maxiter; ++iter)
    {
        alpha = rrk / DOT (p, Ap);

        if (std::isnan (alpha))
        {
            ERROR << "iter [" << iter << "] alpha is a nan.... --> EXIT [err = " << err << "]" << ENDLINE;
            return;
        }

        for (ul_t rowid = 0; rowid < size; ++rowid)
            (*x) [rowid] += alpha * p [rowid];

        for (ul_t rowid = 0; rowid < size; ++rowid)
            r [rowid] -= alpha * Ap [rowid];

        rrk1 = DOT (r, r);

        err = rrk1;
        if (err < m_eps)
            break;

        beta = rrk1 / rrk;

        for (ul_t rowid = 0; rowid < size; ++rowid)
            p [rowid] = r [rowid] + beta * p [rowid];

        for (ul_t rowid = 0; rowid < size; ++rowid)
            Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

        rrk = rrk1;
    }

    //    INFOS << COLOR_BLUE << m_prefix << " Sparse solver : CG " << COLOR_DEFAULT << " [iter = " << iter << " / " <<
    //    size
    //          << ", err = " << err << "/ " << m_eps << "]" << ENDLINE;

    return;
}

void
SparseBiCGSTAB::Solve (const std::vector<real_t> &b, std::vector<real_t> *x)
{
    using Type = std::vector<real_t>;

    ul_t size = SuperClass::m_matrix.Size ();

    if (x->size () != size)
        x->resize (size, 0.0);

    Type r = b;
    for (ul_t rowid = 0; rowid < size; ++rowid)
        r [rowid] -= SuperClass::m_matrix.RowProduct (*x, rowid);

    Type   r0    = r;
    real_t rrk   = DOT (r, r);
    real_t rrk1  = 0.0;
    real_t alpha = 0.0;
    real_t beta  = 0.0;
    real_t omega = 0.0;

    Type p = r;
    Type Ap (size, 0.);

    for (ul_t rowid = 0; rowid < size; ++rowid)
        Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

    Type As (size, 0.0);
    Type s (size, 0.0);

    ul_t   iter;
    real_t err = 1e10;
    for (iter = 0; iter < m_maxiter; ++iter)
    {
        alpha = rrk / DOT (Ap, r0);

        if (std::isnan (alpha))
        {
            ERROR << "iter [" << iter << "] alpha is a nan....  --> EXIT [err = " << err << "]" << ENDLINE;
            return;
        }

        for (ul_t rowid = 0; rowid < size; ++rowid)
            s [rowid] = r [rowid] - alpha * Ap [rowid];

        for (ul_t rowid = 0; rowid < size; ++rowid)
            As [rowid] = SuperClass::m_matrix.RowProduct (s, rowid);

        omega = DOT (As, s) / DOT (As, As);

        if (std::isnan (omega))
        {
            ERROR << "iter [" << iter << "] omega is a nan....  --> EXIT [err = " << err << "]" << ENDLINE;
            return;
        }

        for (ul_t rowid = 0; rowid < size; ++rowid)
            (*x) [rowid] += alpha * p [rowid] + omega * s [rowid];

        for (ul_t rowid = 0; rowid < size; ++rowid)
            r [rowid] = s [rowid] - omega * As [rowid];

        rrk1 = DOT (r, r0);
        err  = DOT (r, r);
        if (err < m_eps)
            break;

        beta = (alpha / omega) * (rrk1 / rrk);

        for (ul_t rowid = 0; rowid < size; ++rowid)
            p [rowid] = r [rowid] + beta * (p [rowid] - omega * Ap [rowid]);

        for (ul_t rowid = 0; rowid < size; ++rowid)
            Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

        rrk = rrk1;
    }

    //    INFOS << COLOR_BLUE << m_prefix << " Sparse solver : BiCGSTAB " << COLOR_DEFAULT << " [iter = " << iter << " /
    //    "
    //          << size << ", err = " << err << "/ " << m_eps << "]" << ENDLINE;

    return;
}
