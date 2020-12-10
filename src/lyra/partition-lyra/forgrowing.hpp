#ifndef SRC_LYRA_PARTITION_LYRA_FORGROWING_HPP
#define SRC_LYRA_PARTITION_LYRA_FORGROWING_HPP

#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "../core/core.hpp"

template <typename T>
error_t
MakeGrowing (Mesh<T> * mesh, std::string filename, ul_t nparts)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        mesh->GetPoint (idPoint)->procsidx = {nparts + 1};

    filename = filename + "." + std::to_string (nparts) + ".log";

    // std::random_device              rd;                    // obtain a random number from hardware
    // std::mt19937                    gen (rd ());           // seed the generator
    // std::uniform_int_distribution<> distr (0, numPoints);  // define the range

    std::vector<ul_t> seedsid (nparts, 0);

    for (ul_t id = 0; id < nparts; ++id)
    {
        seedsid [id]                            = id * static_cast<real_t> (numPoints) / nparts;
        mesh->GetPoint (seedsid [id])->procsidx = {id};
        ERROR << "seed " << id << " " << mesh->GetPoint (seedsid [id])->x << " " << mesh->GetPoint (seedsid [id])->y << " " << seedsid [id] << ENDLINE;
    }

    std::vector<ul_t> card (nparts, 1);
    // card [0]       = numPoints + 1 - nparts;
    ul_t targetmin = numPoints / nparts - numPoints % nparts - 1;
    ul_t targetmax = numPoints / nparts + numPoints % nparts + 1;

    STATUS << "target-min " << targetmin << ENDLINE;
    STATUS << "target-max " << targetmax << ENDLINE;

    // diffusion part
    bool needRunAgain = true;
    int  iter         = 0;

    while (needRunAgain)
    {
        iter++;
        needRunAgain = false;

        std::vector<ul_t> base (numPoints, 0);
        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
            base [idPoint] = mesh->GetPoint (idPoint)->procsidx [0];

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
        {
            // Majoration de la croissance ! Mais peut etre bloqué ensuite...
            if (card [idProc] < targetmax)
            {
                ul_t numofcoloring = 0;
                for (ul_t idPoint = 0; (idPoint < numPoints) && (numofcoloring < 100); ++idPoint)
                {
                    Point<T> * p = mesh->GetPoint (idPoint);

                    if (base [idPoint] == idProc)
                    {
                        for (ul_t idneighs = 0; idneighs < 4; ++idneighs)
                        {
                            Point<T> * neigh = p->neigh [idneighs];

                            if (neigh)  // Le voisin existe
                            {
                                // if (neigh->localId > idPoint)
                                // continue;

                                ul_t * cur = &neigh->procsidx [0];

                                if (*cur >= nparts)  // Attention écrire par defaut si pas encore initialisé
                                {
                                    *cur = idProc;
                                    card [idProc]++;
                                    needRunAgain = true;
                                    numofcoloring++;
                                }
                                else if (card [*cur] > targetmax)  // évite de faire disparaitre des zones
                                {
                                    card [*cur]--;
                                    *cur = idProc;
                                    card [idProc]++;
                                    needRunAgain = true;
                                    numofcoloring++;
                                }

                                // if (card [*cur] < targetmin)  // évite de faire disparaitre des zones
                                // {
                                //     card [*cur]++;
                                //     p->procsidx [0] = *cur;
                                //     card [idProc]--;
                                //     needRunAgain = true;
                                //     numofcoloring++;
                                // }
                            }
                        }
                    }
                }
            }
        }

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
            std::cout << "[" << COLOR_BLUE << idProc << COLOR_DEFAULT << ", #" << card [idProc] << "] " << FLUSHLINE;

        std::cout << ENDLINE;
        // ERROR << iter << " " << std::accumulate (card.begin (), card.end (), 0) << " need to be " << numPoints << ENDLINE;

        if (iter > 1000)
            return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;

    INFOS << COLOR_GREEN << "Iteration de diffusion : " << iter << " pour np : " << numPoints << ENDLINE;

    needRunAgain = true;
    iter         = 0;
    while (needRunAgain)
    {
        iter++;
        needRunAgain = false;

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
        {
            // Majoration de la croissance ! Mais peut etre bloqué ensuite...
            if (card [idProc] < targetmax)
            {
                // This part has the wrong size
                needRunAgain = true;

                for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
                {
                    Point<T> * p = mesh->GetPoint (idPoint);

                    if (p->procsidx [0] == idProc)
                    {
                        for (ul_t idneighs = 0; idneighs < 4; ++idneighs)
                        {
                            Point<T> * neigh = p->neigh [idneighs];

                            // Le voisin existe
                            if (neigh)
                            {
                                ul_t * cur = &neigh->procsidx [0];

                                if (card [*cur] >= targetmax)  // évite de faire disparaitre des zones
                                {
                                    card [*cur]--;
                                    *cur = idProc;
                                    card [idProc]++;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
            std::cout << "[" << COLOR_BLUE << idProc << COLOR_DEFAULT << ", #" << card [idProc] << "] " << FLUSHLINE;

        std::cout << ENDLINE;

        if (iter > 1000)
            break;
    }

    return EXIT_SUCCESS;
}
#endif /* SRC_LYRA_PARTITION_LYRA_FORGROWING_HPP */
