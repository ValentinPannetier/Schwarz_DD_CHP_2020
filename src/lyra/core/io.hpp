#ifndef SRC_LYRA_CORE_IO_HPP
#define SRC_LYRA_CORE_IO_HPP

#include <string>

#include "common.hpp"

class Mesh;

error_t
Read (std::string filename, Mesh * mesh);

error_t
Write (Mesh * mesh, std::string filename);

error_t
WriteBBOnProcs (Mesh * mesh, std::string filename);

// for lyra-partitions
error_t
WriteLyraPartitions (Mesh * mesh, ul_t nparts, std::string basename);
#endif /* SRC_LYRA_CORE_IO_HPP */
