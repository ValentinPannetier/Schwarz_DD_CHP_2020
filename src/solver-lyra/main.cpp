#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>

#include "../core-mpi/core-mpi.hpp"
#include "../core/core.hpp"
#include "solver.hpp"

real_t  dx         = 1.0;
real_t  dy         = 1.0;
real_t  D          = 1.0;
error_t error      = EXIT_SUCCESS;
real_t  epsilon    = 1e-15;
ul_t    numSchwarz = 500;

//// Analytical solution
// real_t
// uana (Point *p, real_t)
//{
//    return (p->y - p->y * p->y) * (p->x - p->x * p->x);
//};

//// Second member
// real_t
// f (Point *p, real_t)
//{
//    return 2.0 * (p->x - p->x * p->x + p->y - p->y * p->y);
//};

//// Boundary condition : you can call uana obviously
// real_t
// g (Point *, real_t)
//{
//    return 0.0;
//};

// Analytical solution
real_t
uana (Point *p, real_t)
{
    return std::sin (p->x) + std::cos (p->y);
};

// Second member
real_t
f (Point *p, real_t)
{
    return std::sin (p->x) + std::cos (p->y);
};

// Boundary condition : you can call uana obviously
real_t
g (Point *p, real_t)
{
    return std::sin (p->x) + std::cos (p->y);
};

int
main (int argc, char **argv)
{
    LyraInit (&argc, &argv);

    if (LYRA_ASK)
    {
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
        std::cout << COLOR_BLUE << REVERSE << "\tWelcome in Solver-Lyra !" << COLOR_DEFAULT << " You are running on "
                  << (ProcMe ? ProcMe->nproc : 0) << " procs." << ENDLINE;
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
    }

    if (argc < 2)
    {
        ERROR << "use " << argv [0] << " filename.lyra" << ENDLINE;
        MPI_USE_ERROR (EXIT_FAILURE)
    }

    Mesh        mesh;
    std::string filename = argv [1];

    filename += ".";
    filename += std::to_string (ProcMe->rank);
    filename += ".lyra";

    error = Read (filename, &mesh, USIGNED (ProcMe->nproc));
    MPI_USE_ERROR (error)

    STATUS << COLOR_BLUE << "[" << ProcMe->rank << "] " << COLOR_DEFAULT << "read mesh " << mesh
           << "[D : " << mesh.count_dirichlet << ", N : " << mesh.count_neumann << ", R : " << mesh.count_robin << "]"
           << ENDLINE;

    //    mesh.PrintCommunications (USIGNED (ProcMe->rank));

    LYRA_BARRIER;
    if (LYRA_ASK)
        ENDFUN;

    error = LyraCheckCommunications (&mesh);
    MPI_USE_ERROR (error)
    LYRA_BARRIER;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Build system
    ///

    SparseMatrix        matrix;
    std::vector<real_t> secMember;
    BuildSystem (&matrix, &secMember, &mesh);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Loop on Schwarz
    ///
    ///

    std::vector<real_t> solAna;
    Fun2Vec (&solAna, &mesh, uana);

    std::vector<real_t> solNum;
    ul_t                numIte = IterateSchwarz (&matrix, &secMember, &mesh, &solNum, &solAna);

    LYRA_BARRIER;
    if (LYRA_ASK)
        INFOS << " Schwarz iterations numbers : " << numIte << " [eps = " << epsilon << "]" << ENDLINE << ENDLINE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Post solver
    ///

    BuildErrors (&solNum, &solAna);

    // OUPUT
    size_t pos = filename.find (".lyra");
    if (pos != std::string::npos)
        filename.erase (pos, 5);

    if (LYRA_ASK)
        BEGIN << "Output" << ENDLINE;

    Write (&mesh, filename + ".out.mesh");
    //    WriteBBOnProcs (&mesh, filename + ".out.bb");
    WriteBBVectorOnProcs (&mesh, &solNum, filename + ".out.bb");
    WriteVTKFile (&mesh, filename + ".out.vtk", &solNum, &solAna);

    LyraFinalize ();
    return EXIT_SUCCESS;
}
