#include "solver.hpp"

#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>

#include "../core-mpi/core-mpi.hpp"
#include "../core/core.hpp"

void
BuildSystem (SparseMatrix *matrix, std::vector<real_t> *secMember, Mesh *mesh)
{
    // Generate the matrix of the problem d_t u - D * Laplacian(u) = f on this
    // proc
    //
    // d_t   u ~ (u_i^n+1 - u_i^n) / dt                 --> order 1
    // d_x^2 u ~ (u_{i-1} - 2 u_i + u_{i+1}) / dx^2     --> order 2
    // d_y^2 u ~ (u_{j-1} - 2 u_j + u_{j+1}) / dy^2     --> order 2

    ul_t numOfPoints = mesh->GetNumberOfPoints ();

    matrix->Init (numOfPoints);

    TripletsList listOfTriplets;
    listOfTriplets.reserve (numOfPoints);

    if (LYRA_ASK)
        BEGIN << "Fill matrix" << ENDLINE;

    // Matrice de Base
    for (ul_t id = 0; id < numOfPoints; ++id)
    {
        Point *p = mesh->GetPoint (id);

        real_t c_surdiagx = 0;
        real_t c_surdiagy = 0;
        dx                = 0;
        dy                = 0;

        for (DIR dir : std::array<DIR, 2> ({D_LEFT, D_RIGHT}))
            if (p->neigh [dir] != nullptr)
            {
                dx         = p->x - p->neigh [dir]->x;
                c_surdiagx = -D * (1.0 / (dx * dx));

                listOfTriplets.push_back (Triplet (id, p->neigh [dir]->localId, c_surdiagx));
            }

        for (DIR dir : std::array<DIR, 2> ({D_UP, D_BOTTOM}))
            if (p->neigh [dir] != nullptr)
            {
                dy         = p->y - p->neigh [dir]->y;
                c_surdiagy = -D * (1.0 / (dy * dy));

                listOfTriplets.push_back (Triplet (id, p->neigh [dir]->localId, c_surdiagy));
            }

        real_t c_diag = -2.0 * c_surdiagx - 2.0 * c_surdiagy;

        listOfTriplets.push_back (Triplet (id, id, c_diag));
    }

    matrix->SetFromTriplet (listOfTriplets.begin (), listOfTriplets.end ());

    STATUS << COLOR_BLUE << "[" << ProcMe->rank << "] " << COLOR_DEFAULT
           << "fill matrix with NNZ = " << matrix->NonZeros () << ENDLINE;

    // Second membre de base
    Fun2Vec (secMember, mesh, f);

    LYRA_BARRIER;

    if (LYRA_ASK)
        ENDFUN;

    return;
}

ul_t
IterateSchwarz (SparseMatrix *matrix, std::vector<real_t> *secMember, Mesh *mesh, std::vector<real_t> *solNum,
                std::vector<real_t> *solAna)
{
    if (LYRA_ASK)
        BEGIN << "Schwarz iterations" << ENDLINE;

    SparseSolverBase *solver = nullptr;
    ul_t              cursor = 0;

    ul_t numOfPoints = mesh->GetNumberOfPoints ();
    solNum->resize (numOfPoints, 0x0);
    std::vector<real_t> lastState (numOfPoints, 0x0);

    std::vector<real_t> valuesToSend (mesh->GetTotalNumberOfSenderPoints (), 0x0);

    // Cast all received values by procs
    std::vector<real_t> valuesToImposeVirtual (mesh->GetTotalNumberOfReceiverPoints (), 0x0);

    // physical values on this proc
    std::vector<real_t> valuesToImposePhysical (mesh->GetNumberOfPhysicalPoints (), 0x0);

    // Before we need to compute values to impose physical
    Fun2Vec (&valuesToImposePhysical, mesh->GetPhysicalPoints (), g);

    ul_t it;
    for (it = 0; it < numSchwarz; ++it)
    {
        SparseMatrix        currMatrix    = *matrix;
        std::vector<real_t> currSecMember = *secMember;

        // Physical part
        ApplySeveralAutoDeduce (&currMatrix, &currSecMember, mesh->GetPhysicalPoints (), &valuesToImposePhysical [0]);

        // Virtual Part
        cursor = 0;
        for (ul_t idProc = 0x0; idProc < USIGNED (ProcMe->nproc); ++idProc)
        {
            if (idProc == USIGNED (ProcMe->rank))
                continue;

            ApplySeveralAutoDeduce (&currMatrix, &currSecMember, mesh->GetReceiverPointsOnProc (idProc),
                                    &valuesToImposeVirtual [cursor]);

            cursor += mesh->GetNumberOfReceiverPointsOnProc (idProc);
        }

        lastState = *solNum;

        // Solve Part
        solver = new SparseBiCGSTAB (currMatrix);
        solver->SetPrefix ("[" + std::to_string (ProcMe->rank) + "]");
        solver->SetTolerance (1e-10);
        solver->Solve (currSecMember, solNum);
        delete solver;

        // Auto slice and take good values to send
        cursor = 0;
        for (ul_t idProc = 0; idProc < USIGNED (ProcMe->nproc); ++idProc)
        {
            if (idProc == USIGNED (ProcMe->rank))
                continue;

            GetSeveralAutoDeduce (solNum, mesh->GetSenderPointsOnProc (idProc), &valuesToSend [cursor], idProc);
            cursor += mesh->GetNumberOfSenderPointsOnProc (idProc);
        }

        // Send and Receive
        LyraSend (mesh, valuesToSend);
        LyraRecv (mesh, valuesToImposeVirtual);

        real_t err = -1.0;
        for (ul_t i = 0; i < numOfPoints; ++i)
            err = std::max (err, std::abs (lastState [i] - (*solNum) [i]));

        real_t errsum = 0;
        MPI_Allreduce (&err, &errsum, 1, MPI_REAL_T, MPI_SUM, MPI_COMM_WORLD);

        WriteVTKFile (mesh, "file_" + std::to_string (ProcMe->rank) + "_" + std::to_string (it) + ".vtk", solNum,
                      solAna);

        if (LYRA_ASK)
            INFOS << "It : " << SPC it << " | error : " << SPC errsum << ENDLINE;

        if (errsum < epsilon)
            break;
    }

    return it;
}

void
BuildErrors (std::vector<real_t> *solNum, std::vector<real_t> *solAna)
{
    LYRA_BARRIER;

    if (LYRA_ASK)
        BEGIN << "Build errors" << ENDLINE;

    real_t proc_error_l1 = 0, error_l1 = 0;
    real_t proc_error_l2 = 0, error_l2 = 0;
    real_t proc_error_rela_l2 = 0, error_rela_l2 = 0;

    ul_t sz = solNum->size ();
    for (ul_t id = 0; id < sz; ++id)
    {
        real_t v = std::abs (solAna->at (id) - solNum->at (id));

        proc_error_l1 += v;
        proc_error_l2 += v * v;

        proc_error_rela_l2 += solAna->at (id) * solAna->at (id);
    }

    MPI_Allreduce (&proc_error_l1, &error_l1, 1, MPI_REAL_T, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce (&proc_error_l2, &error_l2, 1, MPI_REAL_T, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce (&proc_error_rela_l2, &error_rela_l2, 1, MPI_REAL_T, MPI_SUM, MPI_COMM_WORLD);

    error_l1      = std::sqrt (error_l1);
    error_l2      = std::sqrt (error_l2);
    error_rela_l2 = error_l2 / std::sqrt (error_rela_l2);

    if (LYRA_ASK)
    {
        INFOS << "error-l1 = " << std::scientific << SPC      error_l1 << ENDLINE;
        INFOS << "error-l2 = " << std::scientific << SPC      error_l2 << ENDLINE;
        INFOS << "error-rela-l2 = " << std::scientific << SPC error_rela_l2 << ENDLINE;
    }
    return;
}
