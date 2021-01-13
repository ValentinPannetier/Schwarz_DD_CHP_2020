#include "cover.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

error_t
MakeRecoveryZone (Mesh *mesh, ul_t nparts, ul_t nrecover)
{
    BEGIN << "Recovery zone : " << nrecover << ENDLINE;
    ul_t numPoints = mesh->GetNumberOfPoints ();

    (void) nrecover;
    (void) mesh;
    (void) nparts;

    // be sure to share the closure
    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point *p = mesh->GetPoint (idPoint);

            // do it only if i'm not on a closure
            if (p->IsOnTheProc (idProc) && p->procsidx.size () == 1 && p->NumberOfNeighsOnProc (idProc) > 1)
                for (Point *neigh : p->neigh)
                    if (neigh && !neigh->IsOnTheProc (idProc))
                        neigh->procsidx.push_back (idProc);
        }
    }

    // iterate
    for (ul_t idCover = 0; idCover < nrecover; ++idCover)
    {
        std::vector<std::vector<ul_t>> base (numPoints);

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
            base [idPoint] = mesh->GetPoint (idPoint)->procsidx;

        for (ul_t idProc = 0; idProc < nparts; ++idProc)
            for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
            {
                Point *p = mesh->GetPoint (idPoint);

                if (std::find (base [idPoint].begin (), base [idPoint].end (), idProc) != base [idPoint].end ())
                {
                    for (Point *neigh : p->neigh)
                        if (neigh && std::find (base [neigh->localId].begin (), base [neigh->localId].end (), idProc) ==
                                         base [neigh->localId].end ())
                        {
                            if (std::find (neigh->procsidx.begin (), neigh->procsidx.end (), idProc) ==
                                neigh->procsidx.end ())
                            {
                                neigh->procsidx.push_back (idProc);
                            }
                        }
                }
            }
    }

    // Correction part : remove all little extra
    for (ul_t idProc = 0; idProc < nparts; ++idProc)
        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point *p = mesh->GetPoint (idPoint);

            // do it only if i'm not on a closure
            if (p->IsOnTheProc (idProc) && p->NumberOfNeighsOnProc (idProc) <= 1)

                p->procsidx.erase (std::remove (std::begin (p->procsidx), std::end (p->procsidx), idProc),
                                   std::end (p->procsidx));
        }

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        if (mesh->GetPoint (idPoint)->procsidx.size () > 1)
            mesh->GetPoint (idPoint)->tag |= PT_SHARED;

    return EXIT_SUCCESS;
}

void
CheckCover (Mesh *mesh, ul_t nparts)
{
    BEGIN << "Check cover" << ENDLINE;

    ul_t numPoints     = mesh->GetNumberOfPoints ();
    ul_t numTakePoints = 0x0;
    ul_t mincharge     = numPoints;
    ul_t maxcharge     = 0x0;

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        if (mesh->GetPoint (idPoint)->procsidx.size () > 0 && mesh->GetPoint (idPoint)->procsidx [0] < nparts)
            numTakePoints++;

    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        ul_t cardalone  = 0;
        ul_t cardshared = 0;

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point *p = mesh->GetPoint (idPoint);

            if (!p->IsOnTheProc (idProc))
                continue;

            if (p->tag & PT_SHARED)
                cardshared++;
            else
                cardalone++;
        }

        mincharge = std::min (mincharge, cardalone + cardshared);
        maxcharge = std::max (maxcharge, cardalone + cardshared);

        if (cardalone < 3)
            WARNING << "the proc " << COLOR_BLUE << idProc << COLOR_YELLOW << " has only " << COLOR_GREEN << cardalone
                    << COLOR_YELLOW << " exclusive points [" << COLOR_RED << cardshared << COLOR_YELLOW
                    << " shared points]." << ENDLINE;
        else
            INFOS << "the proc " << COLOR_BLUE << idProc << COLOR_DEFAULT << " has " << COLOR_GREEN << cardalone
                  << COLOR_DEFAULT << " exclusive points [" << COLOR_RED << cardshared << COLOR_DEFAULT
                  << " shared points]." << ENDLINE;
    }

    if (numTakePoints != numPoints)
        WARNING << "It seems that an error has occurred ! [np = " << numPoints
                << " and not taken : " << numPoints - numTakePoints << "]." << ENDLINE;
    else
        INFOS << COLOR_GREEN << "All points are taken ! [np = " << numPoints << "]" << ENDLINE;

    INFOS << "Charge : max = " << COLOR_BLUE << maxcharge << COLOR_DEFAULT;
    std::cout << " min = " << COLOR_BLUE << mincharge << COLOR_DEFAULT;
    std::cout << " | diff = " << COLOR_RED << maxcharge - mincharge << ENDLINE;

    return;
}

error_t
MakeGrowing (Mesh *mesh, std::string filename, ul_t nparts)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        mesh->GetPoint (idPoint)->procsidx = {nparts + 1};

    filename = filename + "." + std::to_string (nparts) + ".log";

    std::random_device                                       dev;
    std::mt19937                                             rng (dev ());
    std::uniform_int_distribution<std::mt19937::result_type> dist6 (0, numPoints);

    for (ul_t id = 0; id < nparts; ++id)
        mesh->GetPoint (dist6 (rng))->procsidx = {id};

    std::vector<ul_t> card (nparts, 1);

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
            if (card [idProc] < targetmax)
                for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
                {
                    Point *p = mesh->GetPoint (idPoint);

                    if (!p->IsOnTheProc (idProc))
                        continue;

                    for (DIR d : std::vector<DIR> ({D_LEFT, D_RIGHT, D_UP, D_BOTTOM}))
                    {
                        Point *neigh = p->neigh [d];

                        if (!neigh)
                            continue;

                        if (base [neigh->localId] >= nparts)
                        {
                            base [neigh->localId] = idProc;
                            card [idProc]++;
                            needRunAgain = true;
                        }
                    }
                }
            card [idProc] = 0;
        }

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            mesh->GetPoint (idPoint)->procsidx [0] = base [idPoint];
            card [base [idPoint]]++;
        }

        if (iter > 1000)
            return EXIT_SUCCESS;
    }

    INFOS << "Make " << iter << " iterations" << ENDLINE;

    return EXIT_SUCCESS;
}

error_t
MakeDiagram (Mesh *mesh, std::string filename, ul_t nparts)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    filename += "." + std::to_string (nparts) + ".log";

    std::vector<Point> seeds (nparts);
    std::vector<ul_t>  cards (nparts, 1);
    std::vector<Point> barycenters (nparts);

    for (ul_t id = 0; id < nparts; ++id)
    {
        Point *p = mesh->GetPoint (id * numPoints / nparts);

        seeds [id].x       = p->x;
        seeds [id].y       = p->y;
        seeds [id].z       = p->z;
        cards [id]         = 1;
        barycenters [id].x = seeds [id].x;
        barycenters [id].y = seeds [id].y;
        barycenters [id].z = seeds [id].z;
    }

    bool anotherround = true;
    ul_t iter;
    for (iter = 0; (iter < numPoints) && (anotherround); ++iter)
    {
        anotherround = false;

        Point diff;
        for (ul_t id = 0; id < nparts; ++id)
        {
            barycenters [id].x /= cards [id];
            barycenters [id].y /= cards [id];
            barycenters [id].z /= cards [id];

            diff.x = seeds [id].x - barycenters [id].x;
            diff.y = seeds [id].y - barycenters [id].y;
            diff.z = seeds [id].z - barycenters [id].z;

            real_t diffvalue = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            if (diffvalue > 1e-15 || iter == 0)
            {
                anotherround = true;

                seeds [id].x = barycenters [id].x;
                seeds [id].y = barycenters [id].y;
                seeds [id].z = barycenters [id].z;
            }

            seeds [id].procsidx = {id};
            cards [id]          = 1;
        }

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point *p = mesh->GetPoint (idPoint);
            Point  mdiff;
            ul_t   idseedmin = 0;

            mdiff.x = p->x - seeds [idseedmin].x;
            mdiff.y = p->y - seeds [idseedmin].y;
            mdiff.z = p->z - seeds [idseedmin].z;

            real_t mdist = std::sqrt (mdiff.x * mdiff.x + mdiff.y * mdiff.y + mdiff.z * mdiff.z);

            for (ul_t idProc = 0; idProc < nparts; ++idProc)
            {
                Point diff;

                diff.x = p->x - seeds [idProc].x;
                diff.y = p->y - seeds [idProc].y;
                diff.z = p->z - seeds [idProc].z;

                real_t dist = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

                if (dist < mdist)
                {
                    mdist     = dist;
                    idseedmin = idProc;
                }
            }

            p->procsidx = {seeds [idseedmin].procsidx [0]};

            barycenters [idseedmin].x += p->x;
            barycenters [idseedmin].y += p->y;
            barycenters [idseedmin].z += p->z;
            cards [idseedmin]++;
        }
    }

    INFOS << "Make " << iter << " iterations" << ENDLINE;

    return EXIT_SUCCESS;
}

error_t
MakeMetis (Mesh *mesh, std::string filename, ul_t nparts)
{
    error_t     error;
    std::string command;
    ul_t        numPoints = mesh->GetNumberOfPoints ();
    ul_t        numEdges  = mesh->GetNumberOfEdges ();

    std::ofstream outfile (filename + ".metis.graph");

    outfile << SPC numPoints << SPC numEdges / 2 << std::endl;
    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        Point *p = mesh->GetPoint (ptId);

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
    command =
        "cd libs/Metis/bin/ && gpmetis -ptype=kway -contig -minconn "
        "../../../";
    command += filename + ".metis.graph ";
    command += std::to_string (nparts);
    command += " >> ../../../" + filename + ".metis." + std::to_string (nparts) + ".log";
    error = std::system (command.c_str ());
    USE_ERROR (error)

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
        mesh->GetPoint (ptId)->procsidx = {firstproc};
    }

    infile.close ();

    return EXIT_SUCCESS;
}

error_t
MakeScotch (Mesh *mesh, std::string filename, ul_t nparts)
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
        Point *p = mesh->GetPoint (ptId);

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
    USE_ERROR (error)

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
        mesh->GetPoint (ptId)->procsidx = {firstproc};
    }

    infile.close ();

    return EXIT_SUCCESS;
}

error_t
MakeRegularRows (Mesh *mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t   ny = 1;
    Point *p  = mesh->GetPoint (0);
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
        ul_t end = beg + (ny / nparts) - USIGNED (idProc >= value);

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
MakeRegularColumns (Mesh *mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t   nx = 1;
    Point *p  = mesh->GetPoint (0);
    while (p->neigh [D_RIGHT])
    {
        nx++;
        p = p->neigh [D_RIGHT];
    }

    ul_t value = nx % nparts;
    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        ul_t beg = idProc * (nx / nparts) + std::min (idProc, value);
        ul_t end = beg + (nx / nparts) - USIGNED (idProc >= value);

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
MakeRegularCheckerboards (Mesh *mesh, std::string filename, ul_t nparts)
{
    filename = filename + "." + std::to_string (nparts) + ".log";

    ul_t   ny = 1;
    Point *p  = mesh->GetPoint (0);
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
        ul_t endx = begx + (nx / nparts) - USIGNED (idX >= valuex);

        for (ul_t idY = 0; idY < nparts; ++idY)
        {
            ul_t begy = idY * (ny / nparts) + std::min (idY, valuey);
            ul_t endy = begy + (ny / nparts) - USIGNED (idY >= valuey);

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
