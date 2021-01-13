#include "point.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

ProcsInfosPoint::ProcsInfosPoint (Point *p) : sender (0x0), receivers ({}), normals ({})
{
    std::sort (p->procsidx.begin (), p->procsidx.end ());
    std::unique (p->procsidx.begin (), p->procsidx.end ());

    if (!p->IsComplete ())
        return;

    if (p->procsidx.size () == 1)
        return;

    receivers.clear ();
    sender = p->procsidx [0];

    // Rule of the youngest proc for the sender
    for (ul_t id = 0; id < p->procsidx.size (); ++id)
        if (p->IsCompleteOnTheProc (p->procsidx [id]))
        {
            sender = p->procsidx [id];
            break;
        }

    for (ul_t id = 0; id < p->procsidx.size (); ++id)
    {
        // Exclude all potentials sender
        if (p->IsCompleteOnTheProc (p->procsidx [id]))
            continue;

        DIR dir = D_NOT_SET;

        for (DIR d : std::vector<DIR> ({D_UP, D_BOTTOM, D_RIGHT, D_LEFT}))
            if (!p->neigh [d]->IsOnTheProc (p->procsidx [id]))
            {
                dir = d;
                break;
            }
        receivers.push_back (p->procsidx [id]);
        normals.push_back (dir);
    }
}

Point::Point () :
    globalId (0x0),
    localId (0x0),
    tag (PTAG::PT_DEFAULT),
    x (0x0),
    y (0x0),
    z (0x0),
    neigh ({nullptr, nullptr, nullptr, nullptr}),
    procsidx ({})
{
}

Point::~Point () {}

bool
Point::IsComplete ()
{
    return std::all_of (neigh.begin (), neigh.end (), [] (Point *p) { return p; });
}

ul_t
Point::NumberOfNeighs () const
{
    return 4 - USIGNED (std::count (neigh.begin (), neigh.end (), nullptr));
}

ul_t
Point::NumberOfNeighsOnProc (ul_t idproc)
{
    return USIGNED (std::count_if (neigh.begin (), neigh.end (), [idproc] (Point *p) -> bool {
        if (p)
            return p->IsOnTheProc (idproc);
        return false;
    }));
}

bool
Point::CanMakeCellOnProc (ul_t idproc)
{
    if (!neigh [D_RIGHT] || !neigh [D_RIGHT]->neigh [D_UP] || !neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT])
        return false;

    bool find = IsOnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->IsOnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->neigh [D_UP]->IsOnTheProc (idproc);
    find      = find && neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->IsOnTheProc (idproc);
    return find;
}

bool
Point::CanMakeCell () const
{
    if (!neigh [D_RIGHT] || !neigh [D_RIGHT]->neigh [D_UP] || !neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT])
        return false;
    return true;
}

bool
Point::IsOnTheProc (ul_t idproc)
{
    std::sort (procsidx.begin (), procsidx.end ());

    return std::binary_search (procsidx.begin (), procsidx.end (), idproc);
}

bool
Point::IsCompleteOnTheProc (ul_t idproc)
{
    return IsOnTheProc (idproc) && std::all_of (neigh.begin (), neigh.end (), [idproc] (Point *p) {
               if (p != nullptr)
               {
                   return p->IsOnTheProc (idproc);
               }
               return false;
           });
}
