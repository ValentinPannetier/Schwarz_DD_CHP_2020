#include "boundaryconditions.hpp"

#include "enumlist.hpp"
#include "mesh.hpp"
#include "point.hpp"
#include "sparsematrix.hpp"

void
ApplyPhysicalDirichlet (Mesh * mesh, SparseMatrix * sparse, std::vector<real_t> * secmember, std::function<real_t (Point *, real_t)> fun, real_t time)
{
    std::vector<ul_t> * listofpoints = mesh->GetListOfPointsWithTag (PT_P_DIRICHLET);

    for (ul_t idPoint : *listofpoints)
    {
        Point * p = mesh->GetPoint (idPoint);

        RowCompactor * row = sparse->GetRow (idPoint);

        row->value = {1.};
        row->colid = {idPoint};

        secmember->operator[] (idPoint) = fun (p, time);
    }

    return;
}

void
ApplyVirtualDirichlet (Mesh * mesh, SparseMatrix * sparse, std::vector<real_t> * secmember, std::function<real_t (Point *, real_t)> fun, real_t time)
{
    std::vector<ul_t> * listofpoints = mesh->GetListOfPointsWithTag (PT_V_DIRICHLET);

    for (ul_t idPoint : *listofpoints)
    {
        Point * p = mesh->GetPoint (idPoint);

        RowCompactor * row = sparse->GetRow (idPoint);

        row->value = {1.};
        row->colid = {idPoint};

        secmember->operator[] (idPoint) = fun (p, time);
    }

    return;
}