#ifndef SRC_LYRA_CORE_POINT_HPP
#define SRC_LYRA_CORE_POINT_HPP

#include "common.hpp"
#include "enumlist.hpp"

template <typename T>
struct Point
{
    T                 x         = 0.0;
    T                 y         = 0.0;
    T                 z         = 0.0;
    ul_t              globalId  = 0;  // global id
    ul_t              localId   = 0;  // local id
    PTAG              tag       = PTAG::PT_DEFAULT;
    Point<T> *        neigh [4] = {nullptr, nullptr, nullptr, nullptr};
    std::vector<ul_t> procsidx  = {};

    LYRA_INLINE
    Point<T> *
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

    LYRA_HARD_INLINE
    bool
    OnTheProc (ul_t idproc)
    {
        for (ul_t value : procsidx)
            if (idproc == value)
                return true;
        return false;
    }

    LYRA_INLINE
    bool
    IsCompleteOnTheProc (ul_t idproc)
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

    LYRA_INLINE
    bool
    CanMakeCellOnTheProc (ul_t idproc)
    {
        if (IsComplete () == false)
            return false;

        bool find = OnTheProc (idproc);
        find      = find && neigh [D_RIGHT]->OnTheProc (idproc);
        find      = find && neigh [D_RIGHT]->neigh [D_UP]->OnTheProc (idproc);
        find      = find && neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT]->OnTheProc (idproc);

        return find;
    }

    LYRA_INLINE
    bool
    CanMakeCell ()
    {
        if (IsComplete () == false)
            return false;

        bool find = true;
        find      = find && neigh [D_RIGHT];
        find      = find && neigh [D_RIGHT]->neigh [D_UP];
        find      = find && neigh [D_RIGHT]->neigh [D_UP]->neigh [D_LEFT];

        return find;
    }
};

#endif /* SRC_LYRA_CORE_POINT_HPP */
