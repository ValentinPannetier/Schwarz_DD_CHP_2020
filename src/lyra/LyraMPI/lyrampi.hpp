#ifndef LYRAMPI_HPP
#define LYRAMPI_HPP

#include <mpi.h>

#include <ostream>

#include "../lyra_common.hpp"

struct Proc
{
    int rank  = 0;
    int nproc = 0;
};

extern Proc * ProcMe;

std::ostream &
operator<< (std::ostream & flux, const Proc & proc);

void
LyraInit (int * argc, char *** argv);

void
LyraFinalize ();

#endif  // LYRAMPI_HPP