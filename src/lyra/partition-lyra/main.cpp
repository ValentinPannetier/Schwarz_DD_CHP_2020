
#include <cstdlib>
#include <string>

#include "../core/core.hpp"
#include "cover.hpp"
#include "forcustom.hpp"
#include "formetis.hpp"
#include "forscotch.hpp"

enum PARTITIONNER
{
    METIS  = 1,
    SCOTCH = 2,
    CUSTOM = 3
};

static error_t      error;
static int          nparts;
static int          nrecover;
static PARTITIONNER parter;

int
main (int argc, char const ** argv)
{
    if (argc < 4)
    {
        ERROR << "use " << argv [0] << " filename.lyra nparts nrecover [options] " << ENDLINE;
        ERROR << "options    -m : metis  [default]" << ENDLINE;
        ERROR << "           -s : scotch " << ENDLINE;
        // ERROR << "           -l : lyra-custom " << ENDLINE;
        return EXIT_FAILURE;
    }

    nparts   = std::stoi (argv [2]);
    nrecover = std::stoi (argv [3]);

    parter = METIS;
    if (argc > 4)
    {
        if (std::string (argv [4]) == "-s")
            parter = SCOTCH;
        else if (std::string (argv [4]) == "-l")
            parter = CUSTOM;
    }

    std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
    std::cout << COLOR_BLUE << REVERSE << "\tWelcome in Partition-Lyra !" << ENDLINE;
    std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;

    if (parter == METIS)
        std::cout << "METIS \tnparts = " << nparts << "\tnrecover = " << nrecover << ENDLINE;
    else if (parter == SCOTCH)
        std::cout << "SCOTCH \tnparts = " << nparts << "\tnrecover = " << nrecover << ENDLINE;
    else if (parter == CUSTOM)
        std::cout << "LYRA CUSTOM \tnparts = " << nparts << "\tnrecover = " << nrecover << ENDLINE;

    // Partitionner

    // MESH
    Mesh<real_t> mesh;
    std::string  filename = argv [1];

    error = Read (filename, &mesh);
    USE_ERROR (error);

    size_t pos = filename.find (".lyra");
    if (pos != std::string::npos)
        filename.erase (pos, 5);

    if (parter == METIS)
    {
        error = std::system (std::string ("rm -f " + filename + ".metis." + std::to_string (nparts) + ".log").c_str ());
        USE_ERROR (error);

        error = MakeMetis (&mesh, filename, nparts);
        USE_ERROR (error);

        std::cout << "--> done [log in '" << filename << ".metis." << nparts << ".log']" << ENDLINE;
    }
    else if (parter == SCOTCH)
    {
        error = MakeScotch (&mesh, filename, nparts);
        USE_ERROR (error);

        std::cout << "--> done" << ENDLINE;
    }
    else if (parter == CUSTOM)
    {
        error = MakeCustom (&mesh, filename, nparts);
        USE_ERROR (error);
        std::cout << "--> done" << ENDLINE;
    }

    // Une fois qu'on a partitionné on doit étendre les zones de procs, pour créer des recouvrements.
    error = MakeRecoveryZone (&mesh, nparts, nrecover);
    USE_ERROR (error);

    error = CheckCover (&mesh, nparts);
    USE_ERROR (error);

    // error = WriteLyraPartitions (&mesh, nparts, filename + "." + std::to_string (nparts));
    // USE_ERROR (error);

    error = Write (&mesh, filename + "." + std::to_string (nparts) + ".mesh");
    USE_ERROR (error);

    error = WriteBBOnProcs (&mesh, filename + "." + std::to_string (nparts) + ".bb");
    USE_ERROR (error);

    return EXIT_SUCCESS;
}
