#include "io.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "mesh.hpp"
#include "point.hpp"
#include "tools.hpp"

error_t
Read (std::string filename, Mesh *mesh, ul_t nproc)
{
    std::ifstream file (filename);

    if (!file.is_open ())
    {
        ERROR << "can not open " << filename << " please check it..." << ENDLINE;
        return EXIT_FAILURE;
    }

    // Points
    ul_t numPoints;
    file >> numPoints >> std::ws;

    mesh->InitAndReserve (numPoints, nproc);

    ul_t numProcs = 0;
    ll_t tag;

    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point *p = new Point;

        file >> p->x >> p->y >> p->z >> p->globalId >> tag;

        //                p->procsidx.resize (1, 0);
        p->tag = CastFromBinaryToDecimal (tag);

        if ((p->tag & PT_DIRICHLET) && !(p->tag & PT_NEUMANN))
            mesh->count_dirichlet++;
        else if (!(p->tag & PT_DIRICHLET) && (p->tag & PT_NEUMANN))
            mesh->count_neumann++;
        else if (p->tag & PT_ROBIN)
            mesh->count_robin++;

        if (p->tag & PT_SEND)
        {
            file >> numProcs;

            p->procsidx.resize (numProcs, 0);
            p->normals.resize (numProcs, D_NOT_SET);

            for (ul_t idprocshared = 0; idprocshared < numProcs; ++idprocshared)
                file >> p->procsidx [idprocshared] >> p->normals [idprocshared];
        }
        if (p->tag & PT_RECEIVE)
        {
            p->procsidx.resize (1, 0);
            p->normals.resize (1, D_NOT_SET);

            file >> p->procsidx [0];
        }

        *mesh += p;
    }

    // Cells
    ul_t numCells;
    file >> numCells >> std::ws;
    mesh->SetNumberOfCells (numCells);

    ul_t   id1, id2, id3, id4;
    Point *pt1, *pt2, *pt3, *pt4;

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

    mesh->Finalize ();

    return EXIT_SUCCESS;
}

void
Write (Mesh *mesh, std::string filename)
{
    // MEDIT FILE
    // ne pas mettre les uppercase ni la notation scientifique, il ne les lit
    // pas
    std::ofstream file (filename);

    // file.flags (std::ios::uppercase);
    // file << std::scientific;

    file << "MeshVersionFormatted " << 2 << "\n";
    file << "\nDimension " << 2 << "\n";

    ul_t numPoints = mesh->GetNumberOfPoints ();
    file << "\nVertices\n" << numPoints << std::endl;

    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point *     p = mesh->GetPoint (id);
        file << SPC p->x << SPC p->y << SPC 0 << std::endl;
    }

    file << "\nQuadrilaterals\n";
    file << mesh->GetNumberOfCells () << std::endl;
    for (ul_t id = 0; id < numPoints; ++id)
    {
        Point *p = mesh->GetPoint (id);

        if (p->CanMakeCell ())
        {
            file << SPC p->localId + 1 << SPC p->neigh [D_RIGHT]->localId + 1
                 << SPC                       p->neigh [D_RIGHT]->neigh [D_UP]->localId + 1
                 << SPC p->neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->localId + 1 << SPC 1 << std::endl;
        }
    }

    file << "\nEnd" << std::endl;
    file.close ();

    STATUS << filename << " created." << ENDLINE;

    return;
}

void
WriteBBOnProcs (Mesh *mesh, std::string filename)
{
    // MEDIT BB FILE
    // ne pas mettre les uppercase ni la notation scientifique, il ne les lit
    // pas
    std::ofstream file (filename);

    ul_t numPoints = mesh->GetNumberOfPoints ();
    file << "2 1 " << numPoints << " 2 " << std::endl;

    for (ul_t id = 0; id < numPoints; ++id)
        file << mesh->GetPoint (id)->tag << std::endl;

    file.close ();
    STATUS << filename << " created." << ENDLINE;
    return;
}

void
WriteBBVectorOnProcs (Mesh *mesh, std::vector<real_t> *vec, std::string filename)
{
    // MEDIT BB FILE
    // ne pas mettre les uppercase ni la notation scientifique, il ne les lit
    // pas
    std::ofstream file (filename);

    ul_t numPoints = mesh->GetNumberOfPoints ();
    file << "2 1 " << numPoints << " 2 " << std::endl;

    for (ul_t id = 0; id < numPoints; ++id)
        file << (*vec) [id] << std::endl;

    file.close ();
    STATUS << filename << " created." << ENDLINE;
    return;
}

// for lyra-partitions
void
WriteLyraPartitions (Mesh *mesh, ul_t nparts, std::string basename, PTAG tag)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();

    std::vector<ProcsInfosPoint *> procsinfospoints (numPoints, nullptr);
    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
    {
        procsinfospoints [idPoint] = new ProcsInfosPoint (mesh->GetPoint (idPoint));
        //        mesh->GetPoint (idPoint)->globalId = idPoint;
    }

    for (ul_t idProc = 0; idProc < nparts; ++idProc)
    {
        std::ofstream file (basename + "." + std::to_string (idProc) + ".lyra");

        std::vector<Point *>           meshOnProc;
        std::vector<ProcsInfosPoint *> infosOnProcs;
        ul_t                           numCellsOnMesh = 0;

        for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        {
            Point *p = mesh->GetPoint (idPoint);

            if (p->IsOnTheProc (idProc))
            {
                p->localId = meshOnProc.size ();
                meshOnProc.push_back (p);
                infosOnProcs.push_back (procsinfospoints [idPoint]);

                if (p->CanMakeCellOnProc (idProc))
                    numCellsOnMesh++;
            }
        }

        ul_t numPointsOnMesh = meshOnProc.size ();

        // Write Points

        file << numPointsOnMesh << std::endl;
        for (ul_t id = 0; id < numPointsOnMesh; ++id)
        {
            Point *          p     = meshOnProc [id];
            ProcsInfosPoint *infos = infosOnProcs [id];

            // x y z GID TAG
            file << SPC p->x << SPC p->y << SPC p->z << SPC p->globalId << std::flush;

            PTAG tagtemp = tag;
            for (ul_t idreceiver : infos->receivers)
                if (p->NumberOfNeighsOnProc (idreceiver) < 3)
                {
                    tagtemp = PT_DIRICHLET;
                    break;
                }

            if (infos->sender == idProc && !infos->receivers.empty ())
            {
                file << SPC CastFromDecimalToBinary (p->tag | tagtemp | PT_VIRTUAL | PT_SEND)
                     << SPC infos->receivers.size () << std::flush;

                for (ul_t i = 0; i < infos->receivers.size (); ++i)
                    file << SPC infos->receivers [i] << SPC infos->normals [i] << std::flush;

                file << std::endl;
            }
            else if (std::binary_search (infos->receivers.begin (), infos->receivers.end (),
                                         idProc))  // Need to be received by ONE PROC
            {
                file << SPC CastFromDecimalToBinary (p->tag | tagtemp | PT_VIRTUAL | PT_RECEIVE) << SPC infos->sender
                     << std::endl;
            }
            else
                file << SPC CastFromDecimalToBinary (p->tag) << std::endl;
        }
        file << std::endl;

        //        for (ul_t id = 0; id < numPointsOnMesh; ++id)
        //        {
        //            Point *p = meshOnProc [id];

        //            // x y z GID TAG
        //            file << SPC p->x << SPC p->y << SPC p->z << SPC p->globalId << std::flush;

        //            if (p->IsASenderOnProc (idProc, &list))
        //            {
        //                p->tag = PT_DIRICHLET | PT_VIRTUAL | PT_SEND;

        //                file << SPC CastFromDecimalToBinary (p->tag) << SPC list.size () << std::flush;

        //                for (std::pair<ul_t, ul_t> var : list)
        //                    file << SPC var.first << SPC var.second << std::flush;

        //                file << std::endl;
        //            }
        //            else if (p->IsAReceiverOnProc (idProc, &theSender))  // Need to be received by ONE PROC
        //            {
        //                p->tag = PT_DIRICHLET | PT_VIRTUAL | PT_RECEIVE;

        //                file << SPC CastFromDecimalToBinary (p->tag) << SPC theSender << std::endl;
        //            }
        //            else
        //                file << SPC CastFromDecimalToBinary (p->tag) << std::endl;
        //        }
        //        file << std::endl;

        // Write Send Points to WHO and Normal vec...
        // localId CARD {who normal}_1 ...

        // Write Receiver Points from WHO

        // Write cells
        file << numCellsOnMesh << std::endl;
        for (ul_t id = 0; id < numPointsOnMesh; ++id)
        {
            Point *p = meshOnProc [id];

            if (p->CanMakeCellOnProc (idProc))
            {
                file << SPC p->localId << SPC p->neigh [D_RIGHT]->localId
                     << SPC                   p->neigh [D_RIGHT]->neigh [D_UP]->localId
                     << SPC                   p->neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->localId << std::endl;
            }
        }

        STATUS << std::string ((basename + "." + std::to_string (idProc) + ".lyra")) << " created : " << COLOR_GREEN
               << numPointsOnMesh << COLOR_DEFAULT << " points " << COLOR_GREEN << numCellsOnMesh << COLOR_DEFAULT
               << " quads. " << COLOR_BLUE << "[target proc " << idProc << "]" << ENDLINE;

        file.close ();
    }

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
    {
        delete procsinfospoints [idPoint];
        mesh->GetPoint (idPoint)->localId = mesh->GetPoint (idPoint)->globalId;
    }

    procsinfospoints.clear ();

    return;
}

void
WriteVTKFile (Mesh *mesh, std::string filename, std::vector<real_t> *solnum, std::vector<real_t> *solana)
{
    std::ofstream outfile (filename);

    ul_t numPoints = mesh->GetNumberOfPoints ();

    outfile << "# vtk DataFile Version 2.0" << std::endl;
    outfile << filename << ", Lyra output." << std::endl;
    outfile << "ASCII" << std::endl;
    outfile << "DATASET UNSTRUCTURED_GRID" << std::endl;
    outfile << "POINTS " << numPoints << " double" << std::endl;

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
    {
        Point *        p = mesh->GetPoint (idPoint);
        outfile << SPC p->x << SPC p->y << SPC p->z << std::endl;
    }

    outfile << std::endl;

    ul_t numCells = mesh->GetNumberOfCells ();

    outfile << "CELLS " << numCells << " " << numCells * 5 << std::endl;
    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
    {
        Point *p = mesh->GetPoint (idPoint);

        if (p->CanMakeCell ())
        {
            outfile << 4 << SPC p->localId << SPC p->neigh [D_RIGHT]->localId
                    << SPC                        p->neigh [D_RIGHT]->neigh [D_UP]->localId
                    << SPC p->neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->localId << std::endl;
        }
    }

    outfile << std::endl;

    outfile << "CELL_TYPES " << numCells << std::endl;
    for (ul_t id = 0; id < numCells; ++id)
        outfile << 9 << std::endl;

    outfile << "POINT_DATA " << numPoints << std::endl;
    outfile << "SCALARS "
            << "solnum "
            << "double " << std::endl;
    outfile << "LOOKUP_TABLE default" << std::endl;

    for (real_t value : *solnum)
        outfile << value << std::endl;
    outfile << std::endl;

    if (solana)
    {
        outfile << "SCALARS "
                << "solana "
                << "double " << std::endl;
        outfile << "LOOKUP_TABLE default" << std::endl;

        for (real_t value : *solana)
            outfile << value << std::endl;
        outfile << std::endl;

        outfile << "SCALARS "
                << "error_abs "
                << "double " << std::endl;
        outfile << "LOOKUP_TABLE default" << std::endl;

        for (ul_t id = 0; id < solana->size () && id < solnum->size (); ++id)
            outfile << std::abs (solana->at (id) - solnum->at (id)) << std::endl;
        outfile << std::endl;
    }

    outfile << "SCALARS PTAG int " << std::endl;
    outfile << "LOOKUP_TABLE default" << std::endl;

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        outfile << mesh->GetPoint (idPoint)->tag << std::endl;

    outfile << std::endl;

    //    STATUS << filename << " created : " << COLOR_GREEN << numPoints << COLOR_DEFAULT << " points " << COLOR_GREEN
    //           << numCells << COLOR_DEFAULT << " quads. " << ENDLINE;

    outfile.close ();
}
