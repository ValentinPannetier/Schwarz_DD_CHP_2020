#ifndef SRC_LYRA_PARTITION_LYRA_FORREGULARCHECK_HPP
#define SRC_LYRA_PARTITION_LYRA_FORREGULARCHECK_HPP

#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "../core/core.hpp"

template <typename T>
error_t
MakeRegularCheckerboards (Mesh<T> * mesh, std::string filename, ul_t nparts)
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

    ul_t valuex = nx % nparts;
    ul_t valuey = ny % nparts;

    for (ul_t idX = 0; idX < nparts; ++idX)
    {
        ul_t begx = idX * (nx / nparts) + std::min (idX, valuex);
        ul_t endx = begx + (nx / nparts) - static_cast<ul_t> (idX >= valuex);

        for (ul_t idY = 0; idY < nparts; ++idY)
        {
            ul_t begy = idY * (ny / nparts) + std::min (idY, valuey);
            ul_t endy = begy + (ny / nparts) - static_cast<ul_t> (idY >= valuey);

            // Definition du bloc
            for (ul_t i = begx; i <= endx; ++i)
                for (ul_t j = begy; j <= endy; ++j)
                {
                    p = mesh->GetPoint (j * nx + i);

                    p->procsidx = {(idX + idY) % nparts};

                    // while (p->neigh [D_UP])
                    // {
                    //     p           = p->neigh [D_UP];
                    //     p->procsidx = {idProcx};
                    // }
                }
        }
    }

    return EXIT_SUCCESS;
}
#endif /* SRC_LYRA_PARTITION_LYRA_FORREGULARCHECK_HPP */
