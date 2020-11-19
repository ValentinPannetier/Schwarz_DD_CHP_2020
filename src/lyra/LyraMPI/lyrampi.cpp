#include "lyrampi.hpp"

Proc * ProcMe = new Proc ();

std::ostream&
operator<< (std::ostream & flux, const Proc & proc)
{
    flux << "Proc : " << proc.rank << "/" << proc.nproc << std::flush;

    return flux;
}

void
LyraInit (int * argc, char *** argv)
{
    MPI_Init (argc, argv);

    MPI_Comm_size (MPI_COMM_WORLD, &ProcMe->nproc);
    MPI_Comm_rank (MPI_COMM_WORLD, &ProcMe->rank);

    INFOS << *ProcMe << std::endl;


    return;
}

void
LyraFinalize ()
{
    delete ProcMe;
    MPI_Finalize ();

    return;
}