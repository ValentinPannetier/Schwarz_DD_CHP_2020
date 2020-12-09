#ifndef SRC_LYRA_PARTITION_LYRA_FORMETIS_HPP
#define SRC_LYRA_PARTITION_LYRA_FORMETIS_HPP

#include <iomanip>
#include <iostream>
#include <string>

#include "../core/core.hpp"

template <typename T>
error_t
MakeMetis (Mesh<T> * mesh, std::string filename, ul_t nparts)
{
    error_t     error;
    std::string command;
    ul_t        numPoints = mesh->GetNumberOfPoints ();
    ul_t        numEdges  = mesh->GetNumberOfEdges ();

    std::ofstream outfile (filename + ".metis.graph");

    outfile << SPC numPoints << SPC numEdges / 2 << std::endl;
    for (ul_t ptId = 0; ptId < numPoints; ++ptId)
    {
        Point<real_t> * p = mesh->GetPoint (ptId);

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

#endif /* SRC_LYRA_PARTITION_LYRA_FORMETIS_HPP */
