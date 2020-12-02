
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "Data/mesh.hpp"
#include "IO/io.hpp"
#include "LyraMPI/lyrampi.hpp"
#include "SparseMatrix/sparsematrix.hpp"
// #include "Sparsesolver/bicgstb.hpp"
// #include "Sparsesolver/cg.hpp"
#include "Sparsesolver/sparsesolver.hpp"
#include "Tools/tools.hpp"
#include "lyra_common.hpp"

static error_t error;

int
main (int argc, char ** argv)
{
    // LyraInit (&argc, &argv);

    if (LYRA_ASK)
    {
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
        std::cout << COLOR_BLUE << REVERSE << "\tWelcome in Lyra !" << COLOR_DEFAULT << " You are running on " << (LYRA_PROC ? LYRA_PROC->nproc : 0) << " procs." << ENDLINE;
        std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
    }

    // Example of mesh generation
    if (true)
    {
        int nx = 70;
        int ny = 30;

        real_t dx = 1.0 / (nx - 1);
        real_t dy = 1.0 / (ny - 1);

        std::ofstream file ("../test.lyra");

        // file << std::scientific;
        file << nx * ny << std::endl;
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i)
                file << SPC i * dx << SPC j * dy << SPC 0.0 << SPC j * nx + i << SPC 1 << SPC 0 << std::endl;

        file << std::endl;

        file << (nx - 1) * (ny - 1) << std::endl;

        for (int j = 0; j < ny - 1; ++j)
            for (int i = 0; i < nx - 1; ++i)
            {
                ul_t id = j * nx + i;

                file << SPC id << SPC id + 1 << SPC id + nx + 1 << SPC id + nx << std::endl;
            }

        file.close ();
        // exit (EXIT_SUCCESS);
    }

    // TEST
    if (true)
    {
        SparseMatrix<real_t> matrix;
        matrix.Init (3);

        std::vector<Triplet<real_t> > listOfTriplets;

        listOfTriplets.push_back (Triplet<real_t> (0, 2, 5.0));
        listOfTriplets.push_back (Triplet<real_t> (0, 0, 1.0));
        listOfTriplets.push_back (Triplet<real_t> (1, 1, 1e-25));
        listOfTriplets.push_back (Triplet<real_t> (2, 2, 1.0));

        matrix.SetFromTriplet (listOfTriplets.begin (), listOfTriplets.end ());

        // matrix.PrintSparseView ();
        matrix.PrintDenseView ();
        matrix.Pruned ();
        matrix.PrintDenseView ();

        std::vector<real_t> vec = {1, 1, 1};
        std::cout << "Multiplié par : " << vec << std::endl;

        auto out = matrix * vec;

        std::cout << out << std::endl;
        auto b = 3.0 * out;
        std::cout << b << std::endl;
    }

    // Test Solver
    if (true)
    {
        SparseMatrix<real_t> matrix;
        matrix.Init (3);

        std::vector<Triplet<real_t> > listOfTriplets;

        listOfTriplets.push_back (Triplet<real_t> (0, 0, 1.0));
        // listOfTriplets.push_back (Triplet<real_t> (0, 2, 1.0));
        listOfTriplets.push_back (Triplet<real_t> (1, 1, 2.0));
        listOfTriplets.push_back (Triplet<real_t> (2, 2, 3.0));
        listOfTriplets.push_back (Triplet<real_t> (2, 0, 1.0));

        matrix.SetFromTriplet (listOfTriplets.begin (), listOfTriplets.end ());
        matrix.PrintDenseView (std::cout << "A = \n");

        SparseCG<real_t> solver (matrix);
        // SparseBiCGSTAB<real_t> solver (matrix);

        std::vector<real_t> b = {1, 1, 1};

        std::vector<real_t> x      = solver.Solve (b);
        std::vector<real_t> btilde = matrix * x;

        std::cout << "b       : " << b << std::endl;
        std::cout << "sol     : " << x << std::endl;
        std::cout << "btilde  : " << btilde << std::endl;
    }

    // Test Solver
    if (true)
    {
        srand (time (nullptr));

        ul_t Nx = 30;
        ul_t Ny = 30;

        std::cout << "\n----------------------------------" << std::endl;
        std::cout << "Random test solver Nx : " << Nx << " Ny : " << Ny << std::endl;

        SparseMatrix<real_t> matrix;
        matrix.Init (Nx * Ny);

        std::vector<Triplet<real_t> > listOfTriplets;
        std::vector<real_t>           b;

        listOfTriplets.reserve (5 * Nx * Ny);
        b.resize (Nx * Ny, 0.0);

        for (ul_t i = 0; i < (Nx * Ny); ++i)
        {
            // ul_t randN = std::rand () % N / 10;

            if (i - Ny > 0)
                listOfTriplets.push_back (Triplet<real_t> (i, i - Ny, 1));

            if (i + Ny < (Nx * Ny))
                listOfTriplets.push_back (Triplet<real_t> (i, i + Ny, 1));

            if (i - 1 > 0)
                listOfTriplets.push_back (Triplet<real_t> (i, i - 1, 1));

            if (i + 1 < (Nx * Ny))
                listOfTriplets.push_back (Triplet<real_t> (i, i + 1, 1));

            listOfTriplets.push_back (Triplet<real_t> (i, i, -2));

            // for (ul_t j : std::vector<ul_t> ({i - Ny, i - 1, i, i + 1, i + Ny}))
            // {
            //     real_t value = static_cast<real_t> (std::rand () % 100) / 10.0;

            //     listOfTriplets.push_back (Triplet<real_t> (i, j, value));
            //     listOfTriplets.push_back (Triplet<real_t> (j, i, value));  // symetric
            // }

            real_t value = static_cast<real_t> (std::rand () % 100) / 10.0;

            b [i] = value;
        }

        std::cout << "Fill triplets " << std::endl;

        matrix.SetFromTriplet (listOfTriplets.begin (), listOfTriplets.end ());
        std::cout << "Fill matrix " << std::endl;

        matrix.PrintSparseView (std::cout << "A = \n");
        // std::cout << "b       : " << b << std::endl;

        SparseCG<real_t> solver (matrix);
        // SparseBiCGSTAB<real_t> solver (matrix);

        std::vector<real_t> x      = solver.Solve (b);
        std::vector<real_t> btilde = matrix * x;

        for (ul_t i = 0; i < (Nx * Ny); ++i)
            btilde [i] -= b [i];

        // std::cout << "b       : " << b << std::endl;
        // std::cout << "sol     : " << x << std::endl;
        // using Type = std::vector<real_t>;
        std::cout << "|| Ax - b ||_l2  : " << std::sqrt (DOT (btilde, btilde)) << std::endl;
    }

    if (argc < 2)
    {
        ERROR << "use " << argv [0] << " filename.lyra" << std::endl;
        return EXIT_FAILURE;
    }

    Mesh<real_t> mesh;
    std::string  filename = argv [1];

    error = Read (filename, &mesh);
    USE_ERROR (error);

    STATUS << "Mesh " << mesh << std::endl;

    error = Write (&mesh, "test.mesh");
    USE_ERROR (error);

    // LyraFinalize ();
    return EXIT_SUCCESS;
}
