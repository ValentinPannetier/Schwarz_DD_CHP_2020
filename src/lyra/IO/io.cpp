
#include "io.hpp"

#include <fstream>
#include <iomanip>

#include "../lyra_common.hpp"

error_t
Read (std::string filename, Mesh<real_t> * mesh)
{
    std::ifstream file (filename);

    if (!file.is_open ())
    {
        ERROR << "can not open " << filename << "please check it..." << std::endl;
        return EXIT_FAILURE;
    }

    // Points
    ul_t numPoints;
    file >> numPoints >> std::ws;

    mesh->InitAndReserve (numPoints);

    ul_t numTags = 0;
    ul_t tag     = 0;
    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<real_t> * p = new Point<real_t>;

        file >> p->x >> p->y >> p->z >> p->globalId >> numTags;

        if (numTags == 1)
        {
            file >> tag;
            p->tag = static_cast<PTAG> (tag);
        }
        *mesh += p;
    }

    // Cells
    ul_t numCells;
    file >> numCells >> std::ws;
    mesh->SetNumberOfCells (numCells);

    ul_t           id1, id2, id3, id4;
    Point<real_t> *pt1, *pt2, *pt3, *pt4;
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

error_t
Write (Mesh<real_t> * mesh, std::string filename)
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
        Point<real_t> * p = mesh->GetPoint (id);
        file << SPC p->x << SPC p->y << SPC p->z << std::endl;
    }

    file << "\nQuadrilaterals\n";
    file << mesh->GetNumberOfCells () << std::endl;
    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point<real_t> * p = mesh->GetPoint (id);

        if (p->CanForward ())
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

    return EXIT_SUCCESS;
}