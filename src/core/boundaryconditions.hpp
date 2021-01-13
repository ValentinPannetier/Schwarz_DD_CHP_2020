#ifndef SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP
#define SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP

#include <functional>
#include <vector>

#include "common.hpp"

class Mesh;
class Point;
class SparseMatrix;

/**
 * @brief ApplySeveralAutoDeduce : impose the good condition at each point stored, autodeduce ROBIN DIRICHLET or NEUMANN
 * (not tested)
 * @param sparse : the sparse matrix @see SparseMatrix
 * @param secmember : the second member
 * @param atPoints : list of points
 * @param valuesToImpose : real_t pointer with good values to impose
 */
void
ApplySeveralAutoDeduce (SparseMatrix *sparse, std::vector<real_t> *secmember, std::vector<Point *> *atPoints,
                        real_t *valuesToImpose);

/**
 * @brief ApplyDirichlet : subcase when Dirichlet is detected
 * @param sparse : the sparse matrix @see SparseMatrix
 * @param secmember : the second member
 * @param atPoint : the point
 * @param valueToImpose : real_t pointer with the good value to impose
 */
void
ApplyDirichlet (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose);

/**
 * @brief ApplyNeumann : subcase when Neumann is detected
 * @param sparse : the sparse matrix @see SparseMatrix
 * @param secmember : the second member
 * @param atPoint : the point
 * @param valueToImpose : real_t pointer with the good value to impose
 */
void
ApplyNeumann (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose);

/**
 * @brief ApplyRobin : subcase when Robin is detected
 * @param sparse : the sparse matrix @see SparseMatrix
 * @param secmember : the second member
 * @param atPoint : the point
 * @param valueToImpose : real_t pointer with the good value to impose
 */
void
ApplyRobin (SparseMatrix *sparse, std::vector<real_t> *secmember, Point *atPoint, real_t *valueToImpose);

/**
 * @brief GetSeveralAutoDeduce : recover the good condition at each point stored, autodeduce ROBIN DIRICHLET or NEUMANN
 * (not tested)
 * @param solNum : list of numerical solution (in practice it's the n-1 solution where n is the pseudo time
 * @param atPoints : list of points
 * @param valuesToGet : stored values
 * @param idProc : id of the target proc
 */
void
GetSeveralAutoDeduce (std::vector<real_t> *solNum, std::vector<Point *> *atPoints, real_t *valuesToGet, ul_t idProc);

/**
 * @brief GetDirichlet : subcase when Dirichlet is detected
 * @param solNum : list of numerical solutions
 * @param atPoint : the target point
 * @param valueToGet : value to store
 */
void
GetDirichlet (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet);

/**
 * @brief GetNeumann : subcase when Neumann is detected [need the normal vec]
 * @param solNum : list of numerical solutions
 * @param atPoint : the target point
 * @param valueToGet : value to store
 * @param idProc : id of the target proc
 */
void
GetNeumann (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet, ul_t idProc);

/**
 * @brief GetRobin : subcase when Robin is detected [need the normal vec]
 * @param solNum : list of numerical solutions
 * @param atPoint : the target point
 * @param valueToGet : value to store
 * @param idProc : id of the target proc
 */
void
GetRobin (std::vector<real_t> *solNum, Point *atPoint, real_t *valueToGet, ul_t idProc);

#endif /* SRC_LYRA_CORE_BOUNDARYCONDITIONS_HPP */
