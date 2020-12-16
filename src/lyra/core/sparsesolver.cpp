#include "sparsesolver.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "sparsematrix.hpp"

SparseSolverBase::SparseSolverBase (const SparseMatrix & matrix) : m_matrix (matrix)
{
}

SparseSolverBase::~SparseSolverBase () {}

std::vector<real_t>
SparseCG::Solve (const std::vector<real_t> & b)
{
    using Type = std::vector<real_t>;

    real_t eps = 1e-10;

    ul_t size = SuperClass::m_matrix.Size ();

    Type x (size, 0.);

    Type r = b;
    for (ul_t rowid = 0; rowid < size; ++rowid)
        r [rowid] -= SuperClass::m_matrix.RowProduct (x, rowid);

    real_t rrk   = DOT (r, r);
    real_t rrk1  = 0.0;
    real_t alpha = 0.0;
    real_t beta  = 0.0;

    Type p = r;
    Type Ap (size, 0.0);

    for (ul_t rowid = 0; rowid < size; ++rowid)
        Ap [rowid] = SuperClass::m_matrix.RowProduct (p, rowid);

    ul_t   iter;
    real_t err;

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

std::vector<real_t>
SparseBiCGSTAB::Solve (const std::vector<real_t> & b)
{
    using Type = std::vector<real_t>;

    real_t eps = 1e-20;

    ul_t size = SuperClass::m_matrix.Size ();

    Type x (size, 0.);

    Type r = b;
    for (ul_t rowid = 0; rowid < size; ++rowid)
        r [rowid] -= SuperClass::m_matrix.RowProduct (x, rowid);

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
    real_t err;
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