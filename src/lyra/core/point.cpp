#include "point.hpp"

bool
Point::OnTheProc (ul_t idproc)
{
    for (ul_t value : procsidx)
        if (idproc == value)
            return true;
    return false;
}

bool
Point::IsCompleteOnTheProc (ul_t idproc)
{
    if (IsComplete () == false)
        return false;

    bool find = OnTheProc (idproc);

    if (!find)
        return false;

    for (ul_t idneigh = 0; idneigh < 4; ++idneigh)
    {
        find = neigh [idneigh]->OnTheProc (idproc);

        if (!find)
            return false;
    }

    return true;
}

bool
Point::CanMakeCellOnProc (ul_t idproc)
{
    if (!neigh [D_RIGHT])
        return false;

    if (!neigh [D_RIGHT]->neigh [D_UP])
        return false;

    if (!neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT])
        return false;

    bool find = OnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->OnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->neigh [D_UP]->OnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->OnTheProc (idproc);

    return find;
}

bool
Point::CanMakeCell ()
{
    if (!neigh [D_RIGHT])
        return false;

    if (!neigh [D_RIGHT]->neigh [D_UP])
        return false;

    if (!neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT])
        return false;

    bool find = true;
    find      = find && neigh [D_RIGHT];
    find      = find && neigh [D_RIGHT]->neigh [D_UP];
    find      = find && neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT];

    return find;
}

bool
Point::TotalMatchNeighsOnProc (ul_t idproc)
{
    bool find = OnTheProc (idproc);

    for (DIR d : std::vector<DIR> ({D_LEFT, D_RIGHT, D_UP, D_BOTTOM}))
        if (neigh [d])
            find = find && neigh [d]->OnTheProc (idproc);

    return find;
}

bool
Point::MatchProcIdxNeighs ()
{
    std::sort (procsidx.begin (), procsidx.end ());

    for (DIR d : std::vector<DIR> ({D_LEFT, D_RIGHT, D_UP, D_BOTTOM}))
        if (neigh [d])
        {
            std::sort (neigh [d]->procsidx.begin (), neigh [d]->procsidx.end ());

            if (!std::equal (procsidx.begin (), procsidx.end (), neigh [d]->procsidx.begin ()))
                return false;
        }
    return true;
}