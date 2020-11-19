#ifndef IO_HPP
#define IO_HPP

#include "../Data/mesh.hpp"

error_t
Read (std::string filename, Mesh<real_t> * mesh);

error_t
Write (Mesh<real_t> * mesh, std::string filename);

#endif  // IO_HPP