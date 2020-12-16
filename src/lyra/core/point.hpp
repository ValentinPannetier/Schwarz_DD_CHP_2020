#ifndef SRC_LYRA_CORE_POINT_HPP
#define SRC_LYRA_CORE_POINT_HPP

#include <algorithm>
#include <vector>

#include "common.hpp"
#include "enumlist.hpp"

struct Point
{
    real_t            x         = 0x0;
    real_t            y         = 0x0;
    real_t            z         = 0x0;
    ul_t              globalId  = 0;  // global id
    ul_t              localId   = 0;  // local id
    PTAG              tag       = PTAG::PT_DEFAULT;
    Point *           neigh [4] = {nullptr, nullptr, nullptr, nullptr};
    std::vector<ul_t> procsidx  = {};

    LYRA_INLINE
    Point *
    operator[] (DIR d)
    {
        return neigh [d];
    }

    LYRA_INLINE
    bool
    IsComplete ()
    {
        return neigh [0] && neigh [1] && neigh [2] && neigh [3];
    }

    LYRA_INLINE
    bool
    CanForward ()
    {
        return neigh [D_RIGHT] && neigh [D_UP];
    }

    LYRA_INLINE
    ul_t
    NumberOfNeighs () const
    {
        ul_t count = 0;

        if (neigh [0])
            count++;
        if (neigh [1])
            count++;
        if (neigh [2])
            count++;
        if (neigh [3])
            count++;

        return count;
    }

    bool
    OnTheProc (ul_t idproc);

    bool
    IsCompleteOnTheProc (ul_t idproc);

    bool
    CanMakeCellOnProc (ul_t idproc);

    bool
    CanMakeCell ();

    bool
    TotalMatchNeighsOnProc (ul_t idproc);

    bool
    MatchProcIdxNeighs ();
};

#endif /* SRC_LYRA_CORE_POINT_HPP */
