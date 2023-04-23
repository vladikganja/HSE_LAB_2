#include "generators.hpp"
#include <random>

auto generateRects(uint64_t n) -> std::vector<
    std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>> {
    typedef std::pair<int64_t, int64_t> point;
    typedef std::pair<point, point> rect;
    std::vector<rect> rects;
    for (uint64_t i = 0; i < n; ++i) {
        rects.push_back(
            {{10 * i, 10 * i}, {10 * (n * 2 - i), 10 * (n * 2 - i)}});
    }
    return rects;
}

auto generatePoints(uint64_t n) -> std::vector<std::pair<int64_t, int64_t>> {
    std::mt19937_64 rngX(42), rngY(13);

    int64_t minX = -10;
    int64_t maxX = 20 * static_cast<int64_t>(n) + 10;
    int64_t minY = -10;
    int64_t maxY = 20 * static_cast<int64_t>(n) + 10;
    std::uniform_int_distribution<int64_t> rndX(minX, maxX);
    std::uniform_int_distribution<int64_t> rndY(minY, maxY);

    typedef std::pair<int64_t, int64_t> point;
    std::vector<point> points;
    for (uint64_t i = 0; i < n; ++i) {
        points.emplace_back(rndX(rngX), rndY(rngY));
    }

    return points;
}
