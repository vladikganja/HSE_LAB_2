#ifndef GENERATORS_HPP
#define GENERATORS_HPP
#include <cstdint>
#include <utility>
#include <vector>

auto generateRects(uint64_t n) -> std::vector<
    std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>>;
auto generatePoints(uint64_t n) -> std::vector<std::pair<int64_t, int64_t>>;

#endif  // GENERATORS_HPP
