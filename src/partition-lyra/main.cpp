#include <cstdlib>
#include <string>

#include "../core/core.hpp"
#include "cover.hpp"

static char message [] =
    "|- ptag          -vd, --dirichlet    : Dirichlet boundary condition between partitions\n"
    "|                -vr, --robin        : Robin     boundary condition between partitions\n"
    "|- partitionner  -pm, --metis        : Metis \n"
    "|                -ps, --scotch       : Scotch \n"
    "|                -pv, --voronoi      : equilibrate diagram of Voronoï \n"
    "|                -pc, --column       : regular columns \n"
    "|                -pr, --row          : regular rows \n"
    "|                -pb, --board        : regular checkerboards ";

enum PARTITIONNER
{
    METIS   = 1,
    SCOTCH  = 2,
    DIAGRAM = 4,
    RCOLS   = 5,
    RROWS   = 6,
    RCHECK  = 7
};

static error_t      error;
static ul_t         nparts;
static ul_t         nrecover;
static PARTITIONNER parter;
static std::string  sparter;
static PTAG         virtualtag;
static std::string  svirtualtag;

error_t
ParseArguments (int argc, char const **argv)
{
    if (argc < 6)
    {
        ERROR << "use " << argv [0] << " filename.lyra nparts nrecover $ptag $partitionner " << ENDLINE;
        std::cerr << message << std::endl;

        return EXIT_FAILURE;
    }

    nparts   = USIGNED (std::stoi (argv [2]));
    nrecover = USIGNED (std::stoi (argv [3]));

    if (nparts == 0)
    {
        ERROR << "You need to choose at least 1 partition !" << ENDLINE;
        return EXIT_FAILURE;
    }

    std::string temp = argv [4];

    if (temp == "-vd" || temp == "--dirichlet")
    {
        virtualtag  = PT_DIRICHLET;
        svirtualtag = "Dirichlet";
    }
    else if (temp == "-vr" || temp == "--robin")
    {
        virtualtag  = PT_ROBIN;
        svirtualtag = "Robin";
    }
    else
    {
        ERROR << "unknown parameter " << temp << ENDLINE;
        std::cerr << message << std::endl;
        return EXIT_FAILURE;
    }

    temp = argv [5];

    if (temp == "-pm" || temp == "--metis")
    {
        parter  = METIS;
        sparter = "Metis";
    }
    else if (temp == "-ps" || temp == "--scotch")
    {
        parter  = SCOTCH;
        sparter = "Scotch";
    }
    else if (temp == "-pv" || temp == "--voronoi")
    {
        parter  = DIAGRAM;
        sparter = "Equilibrate Diagram of Voronoï";
    }
    else if (temp == "-pc" || temp == "--column")
    {
        parter  = RCOLS;
        sparter = "Regular Columns";
    }
    else if (temp == "-pr" || temp == "--row")
    {
        parter  = RROWS;
        sparter = "Regular Rows";
    }
    else if (temp == "-pb" || temp == "--board")
    {
        parter  = RCHECK;
        sparter = "Regular Checkerboards";
    }
    else
    {
        ERROR << "unknown parameter " << temp << ENDLINE;
        ERROR << "use " << argv [0] << " filename.lyra nparts nrecover $ptag $partitionner" << ENDLINE;

        std::cerr << message << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

error_t
main (int argc, char const **argv)
{
    error = ParseArguments (argc, argv);
    USE_ERROR (error)

    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;
    std::cout << COLOR_BLUE << REVERSE << " PARTITION-LYRA " << ENDLINE;
    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;

    BEGIN << "Parameters " << ENDLINE;
    INFOS << " algo       = " << COLOR_YELLOW << sparter << ENDLINE;
    INFOS << " nparts     = " << COLOR_YELLOW << nparts << ENDLINE;
    INFOS << " nrecover   = " << COLOR_YELLOW << nrecover << ENDLINE;
    INFOS << " virtualtag = " << COLOR_YELLOW << svirtualtag << ENDLINE;

    // MESH
    Mesh        mesh;
    std::string filename = argv [1];

    error = Read (filename, &mesh, nparts);
    USE_ERROR (error)

    size_t pos = filename.find (".lyra");
    if (pos != std::string::npos)
        filename.erase (pos, 5);

    error = std::system (std::string ("rm -f " + filename + "." + std::to_string (nparts) + ".log").c_str ());
    USE_ERROR (error)

    if (parter == METIS)
        error = MakeMetis (&mesh, filename, nparts);
    else if (parter == SCOTCH)
        error = MakeScotch (&mesh, filename, nparts);
    else if (parter == DIAGRAM)
        error = MakeDiagram (&mesh, filename, nparts);
    else if (parter == RROWS)
        error = MakeRegularRows (&mesh, filename, nparts);
    else if (parter == RCOLS)
        error = MakeRegularColumns (&mesh, filename, nparts);
    else if (parter == RCHECK)
        error = MakeRegularCheckerboards (&mesh, filename, nparts);

    USE_ERROR (error)

    //    std::cout << "--> done [log in '" << filename << "." << nparts << ".log']" << ENDLINE;

    // Une fois qu'on a partitionné on doit étendre les zones de procs, pour
    // créer des recouvrements.
    error = MakeRecoveryZone (&mesh, nparts, nrecover);
    USE_ERROR (error)

    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;
    CheckCover (&mesh, nparts);

    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;
    WriteLyraPartitions (&mesh, nparts, filename + "." + std::to_string (nparts), virtualtag);

    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;
    Write (&mesh, filename + "." + std::to_string (nparts) + ".mesh");

    std::cout << COLOR_BLUE << std::string (80, '-') << ENDLINE;
    WriteBBOnProcs (&mesh, filename + "." + std::to_string (nparts) + ".bb");

    return EXIT_SUCCESS;
}
