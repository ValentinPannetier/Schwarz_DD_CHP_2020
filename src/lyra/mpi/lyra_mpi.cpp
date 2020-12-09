#include "lyra_mpi.hpp"

Proc * LYRA_PROC = nullptr;

std::ostream &
operator<< (std::ostream & flux, const Proc & proc)
{
    flux << "Proc : " << proc.rank << "/" << proc.nproc << std::flush;

    return flux;
}

void
LyraInit (int * argc, char *** argv)
{
    LYRA_PROC = new Proc ();

    MPI_Init (argc, argv);

    MPI_Comm_size (MPI_COMM_WORLD, &LYRA_PROC->nproc);
    MPI_Comm_rank (MPI_COMM_WORLD, &LYRA_PROC->rank);

    return;
}

void
LyraFinalize ()
{
    delete LYRA_PROC;
    MPI_Finalize ();

    return;
}