#ifndef SRC_LYRA_CORE_IO_HPP
#define SRC_LYRA_CORE_IO_HPP

#include <string>
#include <vector>

#include "common.hpp"
#include "enumlist.hpp"

class Mesh;

/**
 * @brief Read a lyra file
 * @param filename
 * @param mesh @see Mesh
 * @param nprocs number of procs (to deduce the file to read)
 * @return an error
 */
error_t
Read (std::string filename, Mesh *mesh, ul_t nprocs = 0x0);

/**
 * @brief Write a mesh file, see Medit files fore more explanation
 * @param mesh @see Mesh
 * @param filename
 */
void
Write (Mesh *mesh, std::string filename);

/**
 * @brief WriteBBOnProcs : see Medit, write the point's tag
 * @param mesh
 * @param filename
 */
void
WriteBBOnProcs (Mesh *mesh, std::string filename);

/**
 * @brief WriteBBVectorOnProcs : see medit and write a data vector
 * @param mesh
 * @param vec
 * @param filename
 */
void
WriteBBVectorOnProcs (Mesh *mesh, std::vector<real_t> *vec, std::string filename);

/**
 * @brief WriteLyraPartitions need to write lyra partition
 * @param mesh
 * @param nparts
 * @param basename
 * @param tag
 */
void
WriteLyraPartitions (Mesh *mesh, ul_t nparts, std::string basename, PTAG tag);

/**
 * @brief WriteVTKFile see VTK
 * @param mesh
 * @param filename
 * @param solnum
 * @param solana
 */
void
WriteVTKFile (Mesh *mesh, std::string filename, std::vector<real_t> *solnum, std::vector<real_t> *solana = nullptr);

#endif /* SRC_LYRA_CORE_IO_HPP */
