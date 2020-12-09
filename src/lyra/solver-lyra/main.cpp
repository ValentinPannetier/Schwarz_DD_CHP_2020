
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#include "../core/core.hpp"
#include "../mpi/lyra_mpi.hpp"

static error_t error;

real_t dt = 1.0;
real_t dx = 1.0;
real_t dy = 1.0;
real_t D  = 1.0;

std::function<real_t (Point<real_t> *, real_t)> f = [] (Point<real_t> * p, real_t t) -> real_t {
    return p->x + p->y + p->z + t;
};

std::function<real_t (Point<real_t> *, real_t)> g = [] (Point<real_t> * p, real_t t) -> real_t {
    return p->x + p->y + p->z + t;
};

std::function<real_t (Point<real_t> *, real_t)> h = [] (Point<real_t> * p, real_t t) -> real_t {
    return p->x + p->y + p->z + t;
};

int
main (int argc, char ** argv)
{
    LyraInit (&argc, &argv);

    if (LYRA_ASK)
    {
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
        std::cout << COLOR_BLUE << REVERSE << "\tWelcome in Solver-Lyra !" << COLOR_DEFAULT << " You are running on " << (LYRA_PROC ? LYRA_PROC->nproc : 0) << " procs." << ENDLINE;
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
    }

    if (argc < 2)
    {
        ERROR << "use " << argv [0] << " filename.lyra" << ENDLINE;
        return EXIT_FAILURE;
    }

    Mesh<real_t> mesh;
    std::string  filename = argv [1];

    filename += ".";
    filename += std::to_string (LYRA_PROC->rank);
    filename += ".lyra";

    error = Read (filename, &mesh);
    USE_ERROR (error);

    STATUS << COLOR_BLUE << "[" << LYRA_PROC->rank << "] " << COLOR_DEFAULT << "read mesh " << mesh << ENDLINE;

    MPI_Barrier (MPI_COMM_WORLD);

    // Generate the matrix of the problem d_t u - D * Laplacian(u) = f on this proc

    // d_t u ~ (u_i^n+1 - u_i^n) / dt                   --> order 1
    // d_x^2 u ~ (u_{i-1} - 2 u_i + u_{i+1}) / dx^2     --> order 2
    // d_y^2 u ~ (u_{j-1} - 2 u_j + u_{j+1}) / dy^2     --> order 2

    real_t c_diag     = 1.0 / dt - D * (-2.0 / (dx * dx) - 2.0 / (dy * dy));
    real_t c_surdiagx = -D * (1.0 / (dx * dx));
    real_t c_surdiagy = -D * (1.0 / (dy * dy));

    SparseMatrix<real_t> matrix;
    std::vector<real_t>  second_member;

    matrix.Init (mesh.GetNumberOfPoints ());
    typename SparseMatrix<real_t>::TripletsList listOfTriplets;

    ul_t numOfPoints = mesh.GetNumberOfPoints ();
    second_member.resize (numOfPoints, 0.0);

    for (ul_t id = 0; id < numOfPoints; ++id)
    {
        Point<real_t> * p = mesh.GetPoint (id);

        listOfTriplets.push_back (Triplet<real_t> (id, id, c_diag));

        for (DIR dir : std::vector<DIR> ({D_UP, D_BOTTOM}))
            if (p->neigh [dir])
                listOfTriplets.push_back (Triplet<real_t> (id, p->neigh [dir]->localId, c_surdiagy));

        for (DIR dir : std::vector<DIR> ({D_LEFT, D_RIGHT}))
            if (p->neigh [dir])
                listOfTriplets.push_back (Triplet<real_t> (id, p->neigh [dir]->localId, c_surdiagx));

        second_member [id] = f (p, 0.0);
    }

    matrix.SetFromTriplet (listOfTriplets.begin (), listOfTriplets.end ());

    STATUS << COLOR_BLUE << "[" << LYRA_PROC->rank << "] " << COLOR_DEFAULT << "fill matrix with NNZ = " << matrix.NonZeros () << ENDLINE;

    // Make Dirichlet condition ?



    // OUPUT
    size_t pos = filename.find (".lyra");
    if (pos != std::string::npos)
        filename.erase (pos, 5);

    MPI_Barrier (MPI_COMM_WORLD);

    if (LYRA_ASK)
    {
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
        INFOS << "Output :" << ENDLINE;
    }

    error = Write (&mesh, filename + ".out.mesh");
    USE_ERROR (error);

    error = WriteBBOnProcs (&mesh, filename + ".out.bb");
    USE_ERROR (error);

    LyraFinalize ();
    return EXIT_SUCCESS;
}
