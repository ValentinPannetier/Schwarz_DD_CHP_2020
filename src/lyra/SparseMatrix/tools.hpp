#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <vector>

template <typename T>
void
apply_permutation (std::vector<T> & in, const std::vector<ul_t> & permu)
{
    std::vector<T> mover;
    mover.reserve (in.size ());
    for (ul_t i = 0; i < in.size (); i++)
        mover.push_back (std::move (in [permu [i]]));

    in = move (mover);
    return;
}

#endif  // TOOLS_HPP