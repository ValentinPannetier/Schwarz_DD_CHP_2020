#include "lyra_mpi.hpp"

Proc *ProcMe = nullptr;

std::ostream &
operator<< (std::ostream &flux, const Proc &proc)
{
    flux << "Proc : " << proc.rank << "/" << proc.nproc << std::flush;

    return flux;
}

void
LyraInit (int *argc, char ***argv)
{
    ProcMe = new Proc ();

    MPI_Init (argc, argv);

    MPI_Comm_size (MPI_COMM_WORLD, &ProcMe->nproc);
    MPI_Comm_rank (MPI_COMM_WORLD, &ProcMe->rank);

    return;
}

void
LyraFinalize ()
{
    delete ProcMe;
    MPI_Finalize ();

    return;
}