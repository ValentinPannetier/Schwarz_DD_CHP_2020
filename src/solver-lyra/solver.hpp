#ifndef SOLVER_HPP
#define SOLVER_HPP
#include <vector>

#include "../core/common.hpp"

extern real_t  dx;
extern real_t  dy;
extern real_t  D;
extern error_t error;
extern real_t  epsilon;
extern ul_t    numSchwarz;

class SparseMatrix;
class Mesh;
class Point;

real_t
uana (Point *p, real_t);

real_t
f (Point *p, real_t);

real_t
g (Point *, real_t);

void
BuildSystem (SparseMatrix *matrix, std::vector<real_t> *secMember, Mesh *mesh);

ul_t
IterateSchwarz (SparseMatrix *matrix, std::vector<real_t> *secMember, Mesh *mesh, std::vector<real_t> *solNum,
                std::vector<real_t> *solAna = nullptr);

void
BuildErrors (std::vector<real_t> *solNum, std::vector<real_t> *solAna);

#endif  // SOLVER_HPP
