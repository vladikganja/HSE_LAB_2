#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <algorithm>

template <typename T>
void compress(std::vector<T> &vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template <typename T>
auto findCIndex(std::vector<T> &vec, T value) -> size_t {
    return static_cast<size_t>(std::upper_bound(vec.begin(), vec.end(), value) -
                               vec.begin() - 1u);
}
#endif  // UTILS_HPP
