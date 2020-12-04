#ifndef SRC_LYRA_MPI_LYRA_MPI_HPP
#define SRC_LYRA_MPI_LYRA_MPI_HPP

#include <mpi.h>

#include <ostream>

#include "../core/common.hpp"

#define LYRA_PROC ProcMe
#define LYRA_ASK  ((LYRA_PROC == nullptr) || (LYRA_PROC->rank == 0))

struct Proc
{
    int rank  = 0;
    int nproc = 0;
};

extern Proc * LYRA_PROC;

std::ostream &
operator<< (std::ostream & flux, const Proc & proc);

void
LyraInit (int * argc, char *** argv);

void
LyraFinalize ();

#endif /* SRC_LYRA_MPI_LYRA_MPI_HPP */
