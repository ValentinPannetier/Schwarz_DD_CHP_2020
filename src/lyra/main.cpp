
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "Data/mesh.hpp"
#include "IO/io.hpp"
#include "LyraMPI/lyrampi.hpp"
#include "SparseMatrix/sparsematrix.hpp"
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

        auto out = matrix * vec;

        for (real_t value : out)
            std::cout << value << " ";
        std::cout << std::endl;
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
