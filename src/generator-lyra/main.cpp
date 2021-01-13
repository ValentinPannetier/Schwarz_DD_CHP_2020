#include <fstream>
#include <iomanip>
#include <iostream>

#include "../core/core.hpp"

static error_t     error;
static ul_t        nx;
static ul_t        ny;
static real_t      lx;
static real_t      ly;
static std::string filename;

error_t
ParseArguments (int argc, char const **argv)
{
    if (argc < 6)
    {
        ERROR << "use " << argv [0] << " lx ly nx ny filename.out " << ENDLINE;
        ERROR << "            lx : length of rectangle on x-axis " << ENDLINE;
        ERROR << "            ly : length of rectangle on y-axis " << ENDLINE;
        ERROR << "            nx : number of points on x-axis" << ENDLINE;
        ERROR << "            ny : number of points on y-axis" << ENDLINE;
        ERROR << " filename.lyra : output filename [like ../data/TheMeshName.lyra] " << ENDLINE;

        return EXIT_FAILURE;
    }

    lx       = std::stod (argv [1]);
    ly       = std::stod (argv [2]);
    nx       = std::stoul (argv [3]);
    ny       = std::stoul (argv [4]);
    filename = std::string (argv [5]);

    return EXIT_SUCCESS;
}

error_t
main (int argc, const char **argv)
{
    error = ParseArguments (argc, argv);
    USE_ERROR (error)

    real_t dx = lx / static_cast<real_t> (nx - 1);
    real_t dy = ly / static_cast<real_t> (ny - 1);

    std::ofstream file (filename);

    // Points
    file << nx * ny << std::endl;

    for (ul_t j = 0; j < ny; ++j)
        for (ul_t i = 0; i < nx; ++i)
        {
            if (i == 0 || j == 0 || j == ny - 1 || i == nx - 1)
                file << SPC static_cast<real_t> (i) * dx << SPC static_cast<real_t> (j) * dy << SPC 0.0
                     << SPC j * nx + i << SPC CastFromDecimalToBinary (PT_DIRICHLET | PT_PHYSICAL) << std::endl;
            else
                file << SPC static_cast<real_t> (i) * dx << SPC static_cast<real_t> (j) * dy << SPC 0.0
                     << SPC j * nx + i << SPC CastFromDecimalToBinary (PT_NONE) << std::endl;
        }
    file << std::endl;

    STATUS << "Write " << nx * ny << " points in " << filename << ENDLINE;

    // Cells
    file << (nx - 1) * (ny - 1) << std::endl;
    for (ul_t j = 0; j < ny - 1; ++j)
        for (ul_t i = 0; i < nx - 1; ++i)
        {
            ul_t id = j * nx + i;

            file << SPC id << SPC id + 1 << SPC id + nx + 1 << SPC id + nx << std::endl;
        }

    STATUS << "Write " << (nx - 1) * (ny - 1) << " cells in " << filename << ENDLINE;

    return EXIT_SUCCESS;
}
