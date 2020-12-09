#ifndef SRC_LYRA_PARTITION_LYRA_FORSCOTCH_HPP
#define SRC_LYRA_PARTITION_LYRA_FORSCOTCH_HPP

#include <iomanip>
#include <iostream>
#include <string>

#include "../core/core.hpp"

template <typename T>
error_t
MakeScotch (Mesh<T> * mesh, std::string filename, ul_t nparts)
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
        Point<T> * p = mesh->GetPoint (ptId);

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
#endif /* SRC_LYRA_PARTITION_LYRA_FORSCOTCH_HPP */
