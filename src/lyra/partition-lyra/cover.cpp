#include "cover.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

error_t
MakeRecoveryZone (Mesh * mesh, ul_t nparts, ul_t nrecover)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    for (ul_t idCover = 0; idCover < nrecover; ++idCover)
    {
        std::vector<std::vector<ul_t>> base (numPoints, std::vector<ul_t> ());
        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
            base [idPoint] = mesh->GetPoint (idPoint)->procsidx;

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
        {
            for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
            {
                Point * p        = mesh->GetPoint (idPoint);
                bool    imonproc = false;

                for (ul_t idprochere : base [idPoint])
                    if (idprochere == idProc)
                    {
                        imonproc = true;
                        break;
                    }

                if (imonproc)
                {
                    for (ul_t idneighs = 0; idneighs < 4; ++idneighs)
                    {
                        Point * neigh = p->neigh [idneighs];
                        if (neigh)
                        {
                            bool found = neigh->OnTheProc (idProc);

                            if (!found)
                                neigh->procsidx.push_back (idProc);
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

error_t
CheckCover (Mesh * mesh, ul_t nparts)
{
    std::cout << COLOR_BLUE << std::string (60, '-') << ENDLINE;

    ul_t   numPoints     = mesh->GetNumberOfPoints ();
    real_t numTakePoints = 0;
    ul_t   mincharge     = numPoints;
    ul_t   maxcharge     = 0;

    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        ul_t cardalone  = 0;
        ul_t cardshared = 0;

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point * p = mesh->GetPoint (idPoint);

            bool imonproc = false;

            for (ul_t idprochere : p->procsidx)
                if (idprochere == idProc)
                {
                    imonproc = true;
                    break;
                }

            if (imonproc)
            {
                if (p->procsidx.size () == 1)
                    cardalone++;
                else
                    cardshared++;

                numTakePoints += 1.0 / static_cast<real_t> (p->procsidx.size ());
            }
        }

        mincharge = std::min (mincharge, cardalone + cardshared);
        maxcharge = std::max (maxcharge, cardalone + cardshared);

        if (cardalone < 3)
            WARNING << "the proc " << COLOR_BLUE << idProc << COLOR_YELLOW << " has only " << COLOR_GREEN << cardalone << COLOR_YELLOW << " exclusive points [" << COLOR_RED << cardshared << COLOR_YELLOW << " shared points]." << ENDLINE;
        else
            INFOS << "the proc " << COLOR_BLUE << idProc << COLOR_DEFAULT << " has " << COLOR_GREEN << cardalone << COLOR_DEFAULT << " exclusive points [" << COLOR_RED << cardshared << COLOR_DEFAULT << " shared points]." << ENDLINE;
    }

    if (static_cast<ul_t> (numTakePoints) != numPoints)
    {
        WARNING << "It seems that an error has occurred ! [np = " << numPoints;
        std::cout << " and not taken : " << numPoints - static_cast<ul_t> (numTakePoints) << "]." << ENDLINE;
    }
    else
        INFOS << COLOR_GREEN << "All points are taken ! [np = " << numPoints << "]" << ENDLINE;

    INFOS << "Charge : max = " << COLOR_BLUE << maxcharge << COLOR_DEFAULT;
    std::cout << " min = " << COLOR_BLUE << mincharge << COLOR_DEFAULT;
    std::cout << " | diff = " << COLOR_RED << maxcharge - mincharge << ENDLINE;

    return EXIT_SUCCESS;
}

error_t
MakeGrowing (Mesh * mesh, std::string filename, ul_t nparts)
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
                    Point * p = mesh->GetPoint (idPoint);

                    if (base [idPoint] == idProc)
                    {
                        for (ul_t idneighs = 0; idneighs < 4; ++idneighs)
                        {
                            Point * neigh = p->neigh [idneighs];

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
                    Point * p = mesh->GetPoint (idPoint);

                    if (p->procsidx [0] == idProc)
                    {
                        for (ul_t idneighs = 0; idneighs < 4; ++idneighs)
                        {
                            Point * neigh = p->neigh [idneighs];

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

error_t
MakeMetis (Mesh * mesh, std::string filename, ul_t nparts)
{
    error_t     error;
    std::string command;
    ul_t        numPoints = mesh->GetNumberOfPoints ();
    ul_t        numEdges  = mesh->GetNumberOfEdges ();

    std::ofstream outfile (filename + ".metis.graph");

    outfile << SPC numPoints << SPC numEdges / 2 << std::endl;
    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        Point * p = mesh->GetPoint (ptId);

        if (p->neigh [0])
            outfile << SPC p->neigh [0]->globalId + 1;
        if (p->neigh [1])
            outfile << SPC p->neigh [1]->globalId + 1;
        if (p->neigh [2])
            outfile << SPC p->neigh [2]->globalId + 1;
        if (p->neigh [3])
            outfile << SPC p->neigh [3]->globalId + 1;

        outfile << std::endl;
    }

    outfile.close ();

    // Call METIS
    command = "cd libs/Metis/bin/ && gpmetis -ptype=kway -contig -minconn ../../../";
    command += filename + ".metis.graph ";
    command += std::to_string (nparts);
    command += " >> ../../../" + filename + ".metis." + std::to_string (nparts) + ".log";
    error = std::system (command.c_str ());
    USE_ERROR (error);

    // Read Results
    std::ifstream infile (filename + ".metis." + "graph.part." + std::to_string (nparts));

    if (!infile.is_open ())
    {
        ERROR << "[internal] can not open the file " << filename << ".metis."
              << ".graph.part." << nparts << ENDLINE;
        return EXIT_FAILURE;
    }

    ul_t firstproc = 0;
    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        infile >> firstproc;
        mesh->GetPoint (ptId)->procsidx [0] = firstproc;
    }

    infile.close ();

    return EXIT_SUCCESS;
}

error_t
MakeScotch (Mesh * mesh, std::string filename, ul_t nparts)
{
    error_t     error;
    std::string command;
    ul_t        numPoints = mesh->GetNumberOfPoints ();
    ul_t        numEdges  = mesh->GetNumberOfEdges ();

    std::ofstream outfile (filename + ".scotch.graph");

    outfile << SPC 0 << std::endl;
    outfile << SPC numPoints << SPC numEdges << std::endl;
    outfile << SPC 0 << SPC "000" << std::endl;

    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        Point * p = mesh->GetPoint (ptId);

        outfile << SPC p->NumberOfNeighs ();
        if (p->neigh [0])
            outfile << SPC p->neigh [0]->globalId;
        if (p->neigh [1])
            outfile << SPC p->neigh [1]->globalId;
        if (p->neigh [2])
            outfile << SPC p->neigh [2]->globalId;
        if (p->neigh [3])
            outfile << SPC p->neigh [3]->globalId;

        outfile << std::endl;
    }

    outfile.close ();

    // Call Scotch
    command = "cd libs/Scotch/bin/ && echo cmplt ";
    command += std::to_string (nparts);
    command += " | ./gmap ../../../";
    command += filename + ".scotch.graph ";
    command += " - ../../../";
    command += filename + ".scotch.graph.part.";
    command += std::to_string (nparts);

    error = std::system (command.c_str ());
    USE_ERROR (error);

    // Read Results
    std::ifstream infile (filename + ".scotch." + "graph.part." + std::to_string (nparts));

    if (!infile.is_open ())
    {
        ERROR << "[internal] can not open the file " << filename << ".scotch."
              << ".graph.part." << nparts << ENDLINE;
        return EXIT_FAILURE;
    }

    ul_t        firstproc = 0;
    std::string trash;
    infile >> trash;
    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        infile >> trash >> firstproc;
        mesh->GetPoint (ptId)->procsidx [0] = firstproc;
    }

    infile.close ();

    return EXIT_SUCCESS;
}

error_t
MakeRegularRows (Mesh * mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t    ny = 1;
    Point * p  = mesh->GetPoint (0);
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

error_t
MakeRegularColumns (Mesh * mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t    nx = 1;
    Point * p  = mesh->GetPoint (0);
    while (p->neigh [D_RIGHT])
    {
        nx++;
        p = p->neigh [D_RIGHT];
    }

    ul_t value = nx % nparts;
    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        ul_t beg = idProc * (nx / nparts) + std::min (idProc, value);
        ul_t end = beg + (nx / nparts) - static_cast<ul_t> (idProc >= value);

        for (ul_t idPoint = beg; idPoint <= end; ++idPoint)
        {
            p = mesh->GetPoint (idPoint);

            p->procsidx = {idProc};

            while (p->neigh [D_UP])
            {
                p           = p->neigh [D_UP];
                p->procsidx = {idProc};
            }
        }
    }

    return EXIT_SUCCESS;
}

error_t
MakeRegularCheckerboards (Mesh * mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t    ny = 1;
    Point * p  = mesh->GetPoint (0);
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