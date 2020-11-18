
#include <iostream>

#include "lyra_common.hpp"
#include "SparseMatrix/sparsematrix.hpp"
#include "Mesh/mesh.hpp"

int main(int, char const **)
{
    real_t a = 2;
    a++;
    ERROR << a << ENDLINE;

    SparseMatrix<real_t> matrix;
    (void)matrix;

    Mesh<real_t> mesh (3, 3);
    ul_t b = mesh.GetGlobalId (1, 2);

    (void)b;
    return 0;
}
