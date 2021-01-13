#ifndef SRC_LYRA_MPI_LYRA_MPI_HPP
#define SRC_LYRA_MPI_LYRA_MPI_HPP

#include <mpi.h>
#include <ostream>

#include "../core/common.hpp"

#define LYRA_ASK ((ProcMe == nullptr) || (ProcMe->rank == 0))

#define LYRA_BARRIER MPI_Barrier (MPI_COMM_WORLD)

struct Proc
{
    int rank  = 0;
    int nproc = 0;
};

extern Proc *ProcMe;

std::ostream &
operator<< (std::ostream &flux, const Proc &proc);

void
LyraInit (int *argc, char ***argv);

void
LyraFinalize ();

#endif /* SRC_LYRA_MPI_LYRA_MPI_HPP */
