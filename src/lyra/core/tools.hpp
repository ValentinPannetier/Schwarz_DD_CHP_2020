#ifndef SRC_LYRA_CORE_TOOLS_HPP
#define SRC_LYRA_CORE_TOOLS_HPP

#include <functional>
#include <ostream>
#include <vector>

#include "mesh.hpp"

template <typename T>
void
apply_permutation (std::vector<T> & in, const std::vector<ul_t> & permu)
{
    std::vector<T> mover;
    mover.reserve (in.size ());
    for (ul_t i = 0; i < in.size (); i++)
        mover.push_back (std::move (in [permu [i]]));

    in = move (mover);
    return;
}

template <typename T>
std::ostream &
operator<< (std::ostream & stream, const std::vector<T> & vec)
{
    for (T value : vec)
        stream << value << " " << std::flush;
    return stream;
}

template <typename T>
T
operator| (const std::vector<T> & a, const std::vector<T> & b)
{
    if (a.size () != b.size ())
        return T ();

    T    result = T ();
    ul_t sz     = a.size ();

    for (ul_t i = 0; i < sz; ++i)
        result += a [i] * b [i];

    return result;
}

template <typename T>
std::vector<T>
operator* (const T & value, const std::vector<T> & vec)
{
    // ul_t           sz     = vec.size ();
    std::vector<T> result = vec;

    for (T & v : result)
        v *= value;

    return result;
}

template <typename T>
std::vector<T>
operator+ (const std::vector<T> & a, const std::vector<T> & b)
{
    if (a.size () != b.size ())
        return std::vector<T> ();

    ul_t           sz     = a.size ();
    std::vector<T> result = a;

    for (ul_t i = 0; i < sz; ++i)
        result [i] += b [i];

    return result;
}

template <typename T>
void
Fun2Vec (std::vector<T> * vec, Mesh * mesh, std::function<T (Point *, T)> fun, T time = T (0x0))
{
    ul_t numPoints = mesh->GetNumberOfPoints ();
    vec->resize (numPoints, T (0x0));

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        vec [idPoint] = fun (mesh->GetPoint (idPoint), time);

    return;
}

template <typename T>
void
Fun2Vec (std::vector<T> * vec, Mesh * mesh, T value = T (0x0))
{
    ul_t numPoints = mesh->GetNumberOfPoints ();
    vec->resize (numPoints, T (0x0));

    for (ul_t idPoint = 0; idPoint < numPoints; ++idPoint)
        vec [idPoint] = value;

    return;
}

#endif /* SRC_LYRA_CORE_TOOLS_HPP */
