#ifndef SRC_LYRA_PARTITION_LYRA_FORREGULARROWS_HPP
#define SRC_LYRA_PARTITION_LYRA_FORREGULARROWS_HPP

#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "../core/core.hpp"

template <typename T>
error_t
MakeRegularRows (Mesh<T> * mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t       ny = 1;
    Point<T> * p  = mesh->GetPoint (0);
    while (p->neigh [D_UP])
    {
        ny++;
        p = p->neigh [D_UP];
    }

    ul_t nx = 1;
    p       = mesh->GetPoint (0);
    while (p->neigh [D_RIGHT])
    {
        nx++;
        p = p->neigh [D_RIGHT];
    }

    ul_t value = ny % nparts;

    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        ul_t beg = idProc * (ny / nparts) + std::min (idProc, value);
        ul_t end = beg + (ny / nparts) - static_cast<ul_t> (idProc >= value);

        for (ul_t idPoint = beg; idPoint <= end; ++idPoint)
        {
            p           = mesh->GetPoint (idPoint * nx);
            p->procsidx = {idProc};

            while (p->neigh [D_RIGHT])
            {
                p           = p->neigh [D_RIGHT];
                p->procsidx = {idProc};
            }
        }
    }

    return EXIT_SUCCESS;
}

#endif /* SRC_LYRA_PARTITION_LYRA_FORREGULARROWS_HPP */
