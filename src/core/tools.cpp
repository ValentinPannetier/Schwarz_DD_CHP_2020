#include "tools.hpp"

#include <cmath>

void
Fun2Vec (std::vector<real_t> *vec, Mesh *mesh, std::function<real_t (Point *, real_t)> fun, real_t time)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();
    vec->resize (numPoints, 0x0);

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        (*vec) [idPoint] = fun (mesh->GetPoint (idPoint), time);

    return;
}

void
Fun2Vec (std::vector<real_t> *vec, Mesh *mesh, real_t value)
{
    ul_t numPoints = mesh->GetNumberOfPoints ();
    vec->resize (numPoints, 0x0);

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        (*vec) [idPoint] = value;

    return;
}

void
Fun2Vec (std::vector<real_t> *vec, std::vector<Point *> *atPoints, std::function<real_t (Point *, real_t)> fun,
         real_t time)
{
    ul_t numPoints = atPoints->size ();
    vec->resize (numPoints, 0x0);

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        (*vec) [idPoint] = fun ((*atPoints) [idPoint], time);

    return;
}

ll_t
CastFromDecimalToBinary (ul_t num)
{
    ll_t out = 0;
    ul_t i   = 1;

    while (num != 0)
    {
        out += (num % 2) * i;
        num /= 2;
        i *= 10;
    }
    return out;
}

ul_t
CastFromBinaryToDecimal (ll_t num)
{
    ul_t out = 0, i = 0;
    while (num != 0)
    {
        out += (num % 10) * USIGNED (std::pow (2, i));
        num /= 10;
        ++i;
    }
    return out;
}
