#ifndef SRC_LYRA_CORE_POINT_HPP
#define SRC_LYRA_CORE_POINT_HPP

#include <algorithm>
#include <sstream>
#include <tuple>
#include <vector>

#include "common.hpp"
#include "enumlist.hpp"

class Point;

struct ProcsInfosPoint
{
    ProcsInfosPoint (Point *p);

    ul_t              sender;
    std::vector<ul_t> receivers;
    std::vector<ul_t> normals;
};

class Point
{
public:
    Point ();

    ~Point ();

    bool
    IsComplete ();

    ul_t
    NumberOfNeighs () const;

    ul_t
    NumberOfNeighsOnProc (ul_t idproc);

    bool
    CanMakeCellOnProc (ul_t idproc);

    bool
    CanMakeCell () const;

    bool
    IsOnTheProc (ul_t idproc);

    bool
    IsCompleteOnTheProc (ul_t idproc);

public:
    // Data
    ul_t                   globalId, localId, tag;
    real_t                 x, y, z;
    std::array<Point *, 4> neigh;
    std::vector<ul_t>      procsidx, normals;
};

#endif /* SRC_LYRA_CORE_POINT_HPP */
