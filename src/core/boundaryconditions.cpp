#include "boundaryconditions.hpp"

#include <algorithm>
#include <cmath>

#include "enumlist.hpp"
#include "mesh.hpp"
#include "point.hpp"
#include "sparsematrix.hpp"
#include "tools.hpp"

void
ApplySeveralAutoDeduce (SparseMatrix *sparse, std::vector<real_t> *secmember, std::vector<Point *> *atPoints,
                        real_t *valuesToImpose)
{
    if (atPoints->size () == 0)
        return;

    for (Point *atPoint : *atPoints)
    {
        if ((atPoint->tag & PT_DIRICHLET) && !(atPoint->tag & PT_NEUMANN))
            ApplyDirichlet (sparse, secmember, atPoint, valuesToImpose);
        else if (!(atPoint->tag & PT_DIRICHLET) && (atPoint->tag & PT_NEUMANN))
            ApplyNeumann (sparse, secmember, atPoint, valuesToImpose);
        else if (atPoint->tag & PT_ROBIN)
            ApplyRobin (sparse, secmember, atPoint, valuesToImpose);

        valuesToImpose++;
    }

    sparse->Pruned ();

    return;
}

void
ApplyDirichlet (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose)
{
    ul_t sizeMat = sparse->Size ();

    //     Operate on the column idPoint
    for (ul_t idRow = 0; idRow < sizeMat; ++idRow)
    {
        RowCompactor *row = sparse->GetRow (idRow);

        ul_t sizeColid  = row->colid.size ();
        ul_t valueColid = atPoint->localId + 1;  // Make sure is not the good value
        ul_t itOnColid  = 0;

        for (; itOnColid < sizeColid; ++itOnColid)
        {
            valueColid = row->colid [itOnColid];
            if (valueColid == atPoint->localId)
                break;
        }

        // End of loop but not the good value
        if (valueColid != atPoint->localId)
            continue;

        secmember->at (idRow) -= row->value [itOnColid] * *valueToImpose;
        row->value [itOnColid] = 0.0;
    }

    // Operate on the row idPoint
    RowCompactor *row = sparse->GetRow (atPoint->localId);

    row->value = {1.0};
    row->colid = {atPoint->localId};  // make only the diagonal coeff equals to 1

    secmember->at (atPoint->localId) = *valueToImpose;

    return;
}

// Order 2 for Neumann -> centered scheme [-1, 0, 1] -> [-1/2, 0, 1/2]
void
ApplyNeumann (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose)
{
    std::vector<DIR> listDir    = {D_LEFT, D_RIGHT, D_UP, D_BOTTOM};
    std::vector<DIR> listInvDir = {D_RIGHT, D_LEFT, D_BOTTOM, D_UP};

    // Operate on the row idPoint
    RowCompactor *row       = sparse->GetRow (atPoint->localId);
    ul_t          sizeColid = row->colid.size ();

    // Scheme
    // u_{ghost} = u_{no-ghost} + 2 * dist * value
    // so
    // coeff for u_{no-ghost} *= 2.0
    // sec-member -= 2 * dist * value

    for (ul_t idir = 0; idir < 4; ++idir)
    {
        Point *pdir  = atPoint->neigh [listDir [idir]];
        Point *pidir = atPoint->neigh [listInvDir [idir]];

        Point diff;
        if (!pidir && pdir)
        {
            diff.x = atPoint->x - pdir->x;
            diff.y = atPoint->y - pdir->y;
            diff.z = atPoint->z - pdir->z;

            real_t dist = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            real_t *coeff = nullptr;

            for (ul_t idCol = 0; idCol < sizeColid; ++idCol)
                if (row->colid [idCol] == pdir->localId)
                {
                    coeff = &row->value [idCol];
                    break;
                }

            // End of loop but not the good value
            if (!coeff)
                continue;

            // Here we assume the scheme for laplacian is symetric
            secmember->at (atPoint->localId) -= *coeff * 2.0 * dist * *valueToImpose;
            *coeff *= 2.0;
        }
    }

    //    STATUS << " Impose Neumann at " << atPoint->localId << " value : " << *valueToImpose << ENDLINE;

    return;
}

void
ApplyRobin (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose)
{
    std::array<DIR, 4> listDir    = {D_LEFT, D_RIGHT, D_UP, D_BOTTOM};
    std::array<DIR, 4> listInvDir = {D_RIGHT, D_LEFT, D_BOTTOM, D_UP};

    // Operate on the row idPoint
    RowCompactor *row       = sparse->GetRow (atPoint->localId);
    ul_t          sizeColid = row->colid.size ();

    // Scheme
    // u_{ghost} = u_{not-ghost} + 2 * dist * value + 2 * dist * u_{to-apply}
    // so
    // coeff for u_{not-ghost} *= 2.0
    // sec-member -= 2 * dist * value * coeff
    // coeff for u_{to-apply} +=  2 * dist * coeff

    for (ul_t idir = 0; idir < 4; ++idir)
    {
        Point *notGhost = atPoint->neigh [listDir [idir]];
        Point *ghost    = atPoint->neigh [listInvDir [idir]];
        Point *toApply  = atPoint;

        Point diff;

        if (!ghost && notGhost)
        {
            diff.x = atPoint->x - notGhost->x;
            diff.y = atPoint->y - notGhost->y;
            diff.z = atPoint->z - notGhost->z;

            real_t dist = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            real_t *coeffNotGhost = nullptr;
            real_t *coeffToApply  = nullptr;

            for (ul_t idCol = 0; idCol < sizeColid; ++idCol)
            {
                if (row->colid [idCol] == notGhost->localId)
                    coeffNotGhost = &row->value [idCol];

                if (row->colid [idCol] == toApply->localId)
                    coeffToApply = &row->value [idCol];
            }

            // End of loop but not the good value
            if (!coeffNotGhost || !coeffToApply)
            {
                ERROR << "can not find" << ENDLINE;
                continue;
            }
            // Scheme
            // u_{ghost} = u_{not-ghost} + 2 * dist * value - 2 * dist * u_{to-apply}
            // so
            // coeff for u_{not-ghost} *= 2.0
            // sec-member -= 2 * dist * value * coeff

            // for example if i-1 --- i+1 with ghost on i-1
            // u_{i-1} = u_{i+1} + 2 * dist(p_{i}, p_{i+1}) * value - 2 * dist(p_{i}, p_{i+1}) * u_i
            // so
            // coeff for u_{i+1} *= 2.0
            // sec-member -= 2 * dist(p_{i}, p_{i+1}) * value * coeff
            // coeff for u_i -=  2 * dist * coeff

            //            INFOS << "id : " << atPoint->localId << " dist = " << dist << ENDLINE;
            //            INFOS << "id : " << atPoint->localId << " valuetoimpose = " << *valueToImpose << ENDLINE;

            //            INFOS << "id : " << atPoint->localId << " BEFORE " << ENDLINE;
            //            for (ul_t idCol = 0; idCol < sizeColid; ++idCol)
            //            {
            //                INFOS << "id : " << atPoint->localId << " col " << row->colid [idCol]
            //                      << " value = " << row->value [idCol] << ENDLINE;
            //            }

            secmember->at (atPoint->localId) -= 2.0 * dist * *valueToImpose * *coeffNotGhost;
            *coeffToApply -= 2.0 * dist * *coeffNotGhost;

            // Here we assume the scheme for laplacian is symetric
            *coeffNotGhost *= 2.0;

            //            INFOS << "id : " << atPoint->localId << " AFTER " << ENDLINE;

            //            for (ul_t idCol = 0; idCol < sizeColid; ++idCol)
            //            {
            //                INFOS << "id : " << atPoint->localId << " col " << row->colid [idCol]
            //                      << " value = " << row->value [idCol] << ENDLINE;
            //            }
            //            INFOS << ENDLINE;

            break;
        }
    }

    //    STATUS << " Impose Robin at " << atPoint->localId << " value : " << *valueToImpose << ENDLINE;

    return;
}

void
GetSeveralAutoDeduce (std::vector<real_t> *solNum, std::vector<Point *> *atPoints, real_t *valuesToGet, ul_t idProc)
{
    if (atPoints->size () == 0)
        return;

    for (Point *atPoint : *atPoints)
    {
        if ((atPoint->tag & PT_DIRICHLET) && !(atPoint->tag & PT_NEUMANN))
            GetDirichlet (solNum, atPoint, valuesToGet);
        else if (!(atPoint->tag & PT_DIRICHLET) && (atPoint->tag & PT_NEUMANN))
            GetNeumann (solNum, atPoint, valuesToGet, idProc);
        else if (atPoint->tag & PT_ROBIN)
            GetRobin (solNum, atPoint, valuesToGet, idProc);

        valuesToGet++;
    }

    return;
}

// u at atPoint
void
GetDirichlet (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet)
{
    *valueToGet = solNum->at (atPoint->localId);

    return;
}

// d_n u at atPoint
void
GetNeumann (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet, ul_t idProc)
{
    // Here, point atPoint must have all its neighboors!
    // Problem what is normal vect ? Look at Normal !

    if (!atPoint->IsComplete ())
    {
        ERROR << "The point " << atPoint->localId << " is not complete... Maybe wrong partitioning ?" << ENDLINE;
        return;
    }

    ul_t normal = D_NOT_SET;
    for (ul_t id = 0; id < atPoint->procsidx.size (); ++id)
    {
        if (atPoint->procsidx [id] == idProc)
            normal = atPoint->normals [id];
    }

    // Loop to find the good vector
    Point *from = nullptr;
    Point *to   = nullptr;

    if (normal == D_UP)
    {
        from = atPoint->neigh [D_BOTTOM];
        to   = atPoint->neigh [D_UP];
    }
    else if (normal == D_BOTTOM)
    {
        from = atPoint->neigh [D_UP];
        to   = atPoint->neigh [D_BOTTOM];
    }
    else if (normal == D_LEFT)
    {
        from = atPoint->neigh [D_RIGHT];
        to   = atPoint->neigh [D_LEFT];
    }
    else if (normal == D_RIGHT)
    {
        from = atPoint->neigh [D_LEFT];
        to   = atPoint->neigh [D_RIGHT];
    }
    else
    {
        ERROR << "The normal is not set for the point " << atPoint->localId << "!" << ENDLINE;
        return;
    }

    real_t u_from = solNum->at (from->localId);

    real_t u_to = solNum->at (to->localId);
    Point  diff;
    diff.x = to->x - from->x;
    diff.y = to->y - from->y;
    diff.z = to->z - from->z;

    real_t dist = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    // scheme is (centered 2)
    // (to - from)/dist = g

    *valueToGet = (u_to - u_from) / dist;
    return;
}

// d_n u + u at atPoint
void
GetRobin (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet, ul_t idProc)
{
    // Here, point atPoint must have all its neighboors !
    // Problem what is normal vect ? Look at Normal !

    if (!atPoint->IsComplete ())
    {
        ERROR << "The point is not complete in " << __FUNCTION__ << "... Maybe wrong partitioning ?" << ENDLINE;
        return;
    }

    ul_t normal = D_NOT_SET;
    for (ul_t id = 0; id < atPoint->procsidx.size (); ++id)
        if (atPoint->procsidx [id] == idProc)
        {
            normal = atPoint->normals [id];
            break;
        }

    // Loop to find the good vector
    Point *not_ghost = nullptr;
    Point *ghost     = atPoint->neigh [normal];

    switch (normal)
    {
        case D_UP: not_ghost = atPoint->neigh [D_BOTTOM]; break;
        case D_BOTTOM: not_ghost = atPoint->neigh [D_UP]; break;
        case D_RIGHT: not_ghost = atPoint->neigh [D_LEFT]; break;
        case D_LEFT: not_ghost = atPoint->neigh [D_RIGHT]; break;
        default: ERROR << "The normal is not set for the point " << atPoint->localId << "!" << ENDLINE; return;
    }

    real_t u_ghost    = solNum->at (ghost->localId);
    real_t u_notGhost = solNum->at (not_ghost->localId);
    real_t u_here     = solNum->at (atPoint->localId);

    Point diff;
    diff.x = not_ghost->x - ghost->x;
    diff.y = not_ghost->y - ghost->y;
    diff.z = not_ghost->z - ghost->z;

    real_t dist = std::sqrt (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    // scheme is (centered 2)
    // (to - from)/dist + here = g

    *valueToGet = (u_ghost - u_notGhost) / dist + u_here;
    return;
}
