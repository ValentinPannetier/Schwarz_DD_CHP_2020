
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Data/mesh.hpp"
#include "IO/io.hpp"
#include "LyraMPI/lyrampi.hpp"
#include "SparseMatrix/sparsematrix.hpp"
#include "lyra_common.hpp"

static error_t error;

int
main (int argc, char ** argv)
{
    LyraInit (&argc, &argv);

    // Example of mesh generation
    if (true)
    {
        int nx = 70;
        int ny = 70;

        real_t dx = 1.0 / (nx - 1);
        real_t dy = 1.0 / (ny - 1);

        std::ofstream file ("../test.lyra");

        // file << std::scientific;
        file << nx * ny << std::endl;
        for (int i = 0; i < nx; ++i)
            for (int j = 0; j < ny; ++j)
                file << SPC i * dx << SPC j * dy << SPC 0.0 << SPC i * nx + j << SPC 1 << SPC 0 << std::endl;

        file << std::endl;

        file << (nx - 1) * (ny - 1) << std::endl;

        for (int i = 0; i < nx - 1; ++i)
            for (int j = 0; j < ny - 1; ++j)
            {
                ul_t id = i * nx + j;

                file << SPC id << SPC id + 1 << SPC id + nx + 1 << SPC id + nx << std::endl;
            }

        file.close ();
        // exit (EXIT_SUCCESS);
    }

    // TEST
    // if (false)
    // {
    //     real_t a = 2;
    //     a++;
    //     ERROR << a << ENDLINE;

    //     SparseMatrix<real_t> matrix;
    //     (void)matrix;

    //     Mesh<real_t> mesh(2, 2);
    //     ul_t b = mesh.GetGlobalId(1, 2);
    //     (void)b;

    //     ul_t i, j;
    //     for (ul_t id : {0, 1, 2, 3, 4, 5})
    //     {
    //         std::tie(i, j) = mesh.GetGridId(id);
    //         std::cout << id << " i:" << i << " j:" << j << std::endl;
    //     }

    //     std::cout << "mesh is " << mesh << std::endl;
    // }

    if (argc < 2)
    {
        ERROR << "use " << argv [0] << " filename.lyra" << std::endl;
        return EXIT_FAILURE;
    }

    Mesh<real_t> mesh;
    std::string  filename = argv [1];

    error = Read (filename, &mesh);
    USE_ERROR (error);

    std::cout << "mesh " << mesh << std::endl;

    error = Write (&mesh, "test.mesh");
    USE_ERROR (error);

    LyraFinalize ();
    return EXIT_SUCCESS;
}
