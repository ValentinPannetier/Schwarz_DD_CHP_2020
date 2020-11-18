#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

#include "../lyra_common.hpp"

template <typename T>
struct Point
{
    T x = 0.0, y = 0.0, z = 0.0;
    ul_t global_id = 0;
};

template <typename T>
class Mesh
{
public:
    Mesh(ul_t nx, ul_t ny) : m_nx(nx), m_ny(ny)
    {
    }

    ~Mesh()
    {
        for (Point<T> *p : m_points)
            delete p;
        m_points.clear();
    }

    LYRA_HARD_INLINE
    ul_t
    GetGlobalId(ul_t i, ul_t j) const
    {
        return i * m_nx + j;
    }

    
protected:
    ul_t m_nx, m_ny, m_nz;
    std::vector<Point<T> *> m_points;

private:
    Mesh(const Mesh &) = delete;
};

#endif // MESH_HPP