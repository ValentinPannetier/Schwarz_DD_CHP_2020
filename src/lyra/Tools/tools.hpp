#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <ostream>
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

template <typename T>
std::ostream &
operator<< (std::ostream & stream, const std::vector<T> & vec)
{
    for (T value : vec)
        stream << value << " " << std::flush;
    return stream;
}
#endif  // TOOLS_HPP