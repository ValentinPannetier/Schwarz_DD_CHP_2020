
#include <cstdlib>
#include <string>

#include "../core/core.hpp"
#include "cover.hpp"

enum PARTITIONNER
{
    METIS   = 1,
    SCOTCH  = 2,
    GROWING = 3,
    RCOLS   = 4,
    RROWS   = 5,
    RCHECK  = 6
};

static error_t      error;
static int          nparts;
static int          nrecover;
static PARTITIONNER parter;
static std::string  sparter;

error_t
ParseArguments (int argc, char const ** argv)
{
    if (argc < 5)
    {
        ERROR << "use " << argv [0] << " filename.lyra nparts nrecover [options] " << ENDLINE;
        ERROR << "options -m  : metis " << ENDLINE;
        ERROR << "        -s  : scotch " << ENDLINE;
        ERROR << "        -g  : growing area with random seeds " << ENDLINE;
        ERROR << "        -rc : regular columns " << ENDLINE;
        ERROR << "        -rr : regular rows " << ENDLINE;
        ERROR << "        -rb : regular checkerboards " << ENDLINE;
        return EXIT_FAILURE;
    }

    nparts   = std::stoi (argv [2]);
    nrecover = std::stoi (argv [3]);

    if (argc > 4)
    {
        if (std::string (argv [4]) == "-s")
        {
            parter  = SCOTCH;
            sparter = "Scotch";
        }
        else if (std::string (argv [4]) == "-g")
        {
            parter  = GROWING;
            sparter = "Growing Area With Random Seeds";
        }
        else if (std::string (argv [4]) == "-rc")
        {
            parter  = RCOLS;
            sparter = "Regular Columns";
        }
        else if (std::string (argv [4]) == "-rr")
        {
            parter  = RROWS;
            sparter = "Regular Rows";
        }
        else if (std::string (argv [4]) == "-rb")
        {
            parter  = RCHECK;
            sparter = "Regular Checkerboards";
        }
        else if (std::string (argv [4]) == "-m")
        {
            parter  = METIS;
            sparter = "Metis";
        }
        else
        {
            ERROR << "unknown option " << COLOR_BLUE << std::string (argv [4]) << ENDLINE;
            WARNING << "use " << argv [0] << " filename.lyra nparts nrecover [options] " << ENDLINE;
            WARNING << "options -m  : metis " << ENDLINE;
            WARNING << "        -s  : scotch " << ENDLINE;
            WARNING << "        -g  : growing area with random seeds " << ENDLINE;
            WARNING << "        -rc : regular columns " << ENDLINE;
            WARNING << "        -rr : regular rows " << ENDLINE;
            WARNING << "        -rb : regular checkerboards " << ENDLINE;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int
main (int argc, char const ** argv)
{
    error = ParseArguments (argc, argv);
    USE_ERROR (error);

    std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;
    std::cout << COLOR_BLUE << REVERSE << "\tWelcome in Partition-Lyra !" << ENDLINE;
    std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;

    std::cout << sparter << "\tnparts = " << nparts << "\tnrecover = " << nrecover << ENDLINE;

    // MESH
    Mesh        mesh;
    std::string filename = argv [1];

    error = Read (filename, &mesh);
    USE_ERROR (error);

    size_t pos = filename.find (".lyra");
    if (pos != std::string::npos)
        filename.erase (pos, 5);

    error = std::system (std::string ("rm -f " + filename + "." + std::to_string (nparts) + ".log").c_str ());
    USE_ERROR (error);

    if (parter == METIS)
        error = MakeMetis (&mesh, filename, nparts);
    else if (parter == SCOTCH)
        error = MakeScotch (&mesh, filename, nparts);
    else if (parter == GROWING)
        error = MakeGrowing (&mesh, filename, nparts);
    else if (parter == RROWS)
        error = MakeRegularRows (&mesh, filename, nparts);
    else if (parter == RCOLS)
        error = MakeRegularColumns (&mesh, filename, nparts);
    else if (parter == RCHECK)
        error = MakeRegularCheckerboards (&mesh, filename, nparts);

    USE_ERROR (error);

    std::cout << "--> done [log in '" << filename << "." << nparts << ".log']" << ENDLINE;

    // Une fois qu'on a partitionné on doit étendre les zones de procs, pour créer des recouvrements.
    error = MakeRecoveryZone (&mesh, nparts, nrecover);
    USE_ERROR (error);

    error = CheckCover (&mesh, nparts);
    USE_ERROR (error);

    error = WriteLyraPartitions (&mesh, nparts, filename + "." + std::to_string (nparts));
    USE_ERROR (error);

    error = Write (&mesh, filename + "." + std::to_string (nparts) + ".mesh");
    USE_ERROR (error);

    error = WriteBBOnProcs (&mesh, filename + "." + std::to_string (nparts) + ".bb");
    USE_ERROR (error);

    return EXIT_SUCCESS;
}
