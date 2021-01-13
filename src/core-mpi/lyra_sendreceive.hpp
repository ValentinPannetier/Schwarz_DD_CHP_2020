#ifndef LYRA_SENDRECEIVE_HPP
#define LYRA_SENDRECEIVE_HPP

#include <vector>

#include "lyra_mpi.hpp"

class Mesh;

/**
 * @brief LyraSend
 * @param mesh
 * @param values
 */
void
LyraSend (Mesh *mesh, std::vector<real_t> &values);

/**
 * @brief LyraRecv, receiver on points-receiver with an auto resize vectors results
 * @param mesh
 * @param values
 */
void
LyraRecv (Mesh *mesh, std::vector<real_t> &values);

error_t
LyraCheckCommunications (Mesh *mesh);

#endif  // LYRA_SENDRECEIVE_HPP
