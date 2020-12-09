#ifndef SRC_LYRA_CORE_IO_HPP
#define SRC_LYRA_CORE_IO_HPP

#include <fstream>
#include <iomanip>
#include <numeric>

#include "common.hpp"
#include "mesh.hpp"

template <typename T>
error_t
Read (std::string filename, Mesh<T> * mesh)
{
    std::ifstream file (filename);

    if (!file.is_open ())
    {
        ERROR << "can not open " << filename << " please check it..." << std::endl;
        return EXIT_FAILURE;
    }

    // Points
    ul_t numPoints;
    file >> numPoints >> std::ws;

    mesh->InitAndReserve (numPoints);

    ul_t numProcs = 0;
    ul_t tag      = 0;
    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<T> * p = new Point<T>;

        file >> p->x >> p->y >> p->z >> p->globalId >> tag >> numProcs;

        p->tag = static_cast<PTAG> (tag);

        p->procsidx.resize (numProcs, 0);
        for (ul_t idprocshared = 0; idprocshared < numProcs; ++idprocshared)
            file >> p->procsidx [idprocshared];

        *mesh += p;
    }

    // Cells
    ul_t numCells;
    file >> numCells >> std::ws;
    mesh->SetNumberOfCells (numCells);

    ul_t      id1, id2, id3, id4;
    Point<T> *pt1, *pt2, *pt3, *pt4;
    //
    //      4 ----- 3
    //      |       |
    //      |       |
    //      1 ----- 2
    //

    for (ul_t id = 0; id < numCells; ++id)
    {
        file >> id1 >> id2 >> id3 >> id4 >> std::ws;
        pt1 = mesh->GetPoint (id1);
        pt2 = mesh->GetPoint (id2);
        pt3 = mesh->GetPoint (id3);
        pt4 = mesh->GetPoint (id4);

        pt1->neigh [D_RIGHT] = pt2;  // 1 --> 2
        pt2->neigh [D_LEFT]  = pt1;  // 1 <-- 2
        pt4->neigh [D_RIGHT] = pt3;  // 4 --> 3
        pt3->neigh [D_LEFT]  = pt4;  // 4 <-- 3

        pt1->neigh [D_UP]     = pt4;
        pt4->neigh [D_BOTTOM] = pt1;
        pt2->neigh [D_UP]     = pt3;
        pt3->neigh [D_BOTTOM] = pt2;
    }

    file.close ();

    return EXIT_SUCCESS;
}

template <typename T>
error_t
Write (Mesh<T> * mesh, std::string filename)
{
    // MEDIT FILE
    // ne pas mettre les uppercase ni la notation scientifique, il ne les lit pas
    std::ofstream file (filename);

    // file.flags (std::ios::uppercase);
    // file << std::scientific;

    file << "MeshVersionFormatted " << 1 << "\n";
    file << "\nDimension " << 2 << "\n";

    ul_t numPoints = mesh->GetNumberOfPoints ();
    file << "\nVertices\n"
         << numPoints << std::endl;

    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<T> *  p = mesh->GetPoint (id);
        file << SPC p->x << SPC p->y << SPC p->z << std::endl;
    }

    file << "\nQuadrilaterals\n";
    file << mesh->GetNumberOfCells () << std::endl;
    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<T> * p = mesh->GetPoint (id);

        if (p->CanMakeCell ())
        {
            file << SPC p->localId + 1
                 << SPC p->neigh [D_RIGHT]->localId + 1
                 << SPC p->neigh [D_RIGHT]->neigh [D_UP]->localId + 1
                 << SPC p->neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->localId + 1
                 << SPC 1
                 << std::endl;
        }
    }

    file << "\nEnd" << std::endl;
    file.close ();

    STATUS << filename << " created." << ENDLINE;

    return EXIT_SUCCESS;
}

template <typename T>
error_t
WriteBBOnProcs (Mesh<T> * mesh, std::string filename)
{
    // MEDIT BB FILE
    // ne pas mettre les uppercase ni la notation scientifique, il ne les lit pas
    std::ofstream file (filename);

    ul_t numPoints = mesh->GetNumberOfPoints ();
    file << "2 1 " << numPoints << " 2 " << std::endl;

    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<T> * p    = mesh->GetPoint (id);
        real_t     mean = std::accumulate (p->procsidx.begin (), p->procsidx.end (), 0) / static_cast<real_t> (p->procsidx.size ());

        // real_t mean = std::accumulate (p->procsidx.begin (), p->procsidx.end (), 1);

        // real_t mean = static_cast<real_t> (p->procsidx.size ());
        file << mean << std::endl;
    }

    file.close ();
    STATUS << filename << " created." << ENDLINE;

    return EXIT_SUCCESS;
}

// for lyra-partitions
template <typename T>
error_t
WriteLyraPartitions (Mesh<T> * mesh, ul_t nparts, std::string basename)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        std::ofstream file (basename + "." + std::to_string (idProc) + ".lyra");

        std::vector<Point<T> *> meshOnProc;
        ul_t                    numCellsOnMesh = 0;

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point<T> * p = mesh->GetPoint (idPoint);

            if (p->OnTheProc (idProc))
            {
                p->localId = meshOnProc.size ();
                meshOnProc.push_back (p);

                if (p->CanMakeCellOnTheProc (idProc))
                    numCellsOnMesh++;
            }
        }

        ul_t numPointsOnMesh = meshOnProc.size ();

        file << numPointsOnMesh << std::endl;

        for (ul_t id = 0; id < numPointsOnMesh; ++id)
        {
            Point<T> *  p = meshOnProc [id];
            file << SPC p->x << SPC p->y << SPC p->z << std::flush;

            file << SPC p->globalId << SPC 0 << SPC p->procsidx.size () << std::flush;

            for (ul_t prochere : p->procsidx)
                file << SPC prochere;

            file << std::endl;
        }

        file << std::endl;
        file << numCellsOnMesh << std::endl;
        for (ul_t id = 0; id < numPointsOnMesh; ++id)
        {
            Point<T> * p = meshOnProc [id];

            if (p->CanMakeCellOnTheProc (idProc))
            {
                file << SPC p->localId
                     << SPC p->neigh [D_RIGHT]->localId
                     << SPC p->neigh [D_RIGHT]->neigh [D_UP]->localId
                     << SPC p->neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->localId
                     << std::endl;
            }
        }

        STATUS << std::string ((basename + "." + std::to_string (idProc) + ".lyra")) << " created." << COLOR_BLUE << "[target proc " << idProc << "]" << ENDLINE;

        file.close ();
    }

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
    {
        Point<T> * p = mesh->GetPoint (idPoint);
        p->localId   = p->globalId;
    }

    return EXIT_SUCCESS;
}
#endif /* SRC_LYRA_CORE_IO_HPP */
