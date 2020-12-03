#ifndef POINT_HPP
#define POINT_HPP

#include "../lyra_common.hpp"
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
        return neigh [0] && neigh [1] && neigh [2] && neigh [4];
    }

    LYRA_INLINE
    bool
    CanForward ()
    {
        return neigh [D_RIGHT] && neigh [D_UP];
    }
};

#endif  // POINT_HPP