#ifndef SRC_LYRA_PARTITION_LYRA_COVER_HPP
#define SRC_LYRA_PARTITION_LYRA_COVER_HPP

#include "../core/core.hpp"

error_t
MakeRecoveryZone (Mesh *mesh, ul_t nparts, ul_t nrecover);

void
CheckCover (Mesh *mesh, ul_t nparts);

error_t
MakeGrowing (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeDiagram (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeMetis (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeScotch (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeRegularRows (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeRegularColumns (Mesh *mesh, std::string filename, ul_t nparts);

error_t
MakeRegularCheckerboards (Mesh *mesh, std::string filename, ul_t nparts);

#endif /* SRC_LYRA_PARTITION_LYRA_COVER_HPP */
