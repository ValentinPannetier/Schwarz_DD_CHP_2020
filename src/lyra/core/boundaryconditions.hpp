#ifndef SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP
#define SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP

#include <functional>
#include <vector>

#include "common.hpp"

class Mesh;
class Point;
class SparseMatrix;

void
ApplyPhysicalDirichlet (Mesh * mesh, SparseMatrix * sparse, std::vector<real_t> * secmember, std::function<real_t (Point *, real_t)> fun, real_t time = 0x0);

void
ApplyVirtualDirichlet (Mesh * mesh, SparseMatrix * sparse, std::vector<real_t> * secmember);

#endif /* SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP */
