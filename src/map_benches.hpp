#ifndef MAP_BENCHES_HPP
#define MAP_BENCHES_HPP

#include "utils.hpp"
#include "benchmark/benchmark.h"
#include "generators.hpp"
#include <algorithm>

auto build_map(std::vector<std::pair<std::pair<int64_t, int64_t>,
                                     std::pair<int64_t, int64_t>>> &rects)
    -> std::tuple<std::vector<std::vector<uint64_t>>, std::vector<int64_t>,
                  std::vector<int64_t>> {
    std::vector<int64_t> cx;
    std::vector<int64_t> cy;
    for (const auto &r : rects) {
        cx.push_back(r.first.first);
        cx.push_back(r.second.first);
        cx.push_back(r.second.first + 1);
        cy.push_back(r.first.second);
        cy.push_back(r.second.second);
        cy.push_back(r.second.second + 1);
    }
    compress(cx);
    compress(cy);

    std::vector<std::vector<uint64_t>> map;
    map.resize(cx.size());
    for (size_t i = 0; i < cx.size(); ++i) {
        map[i].resize(cy.size());
    }

    for (const auto &r : rects) {
        size_t rLFCx = findCIndex(cx, r.first.first);
        size_t rLFCy = findCIndex(cy, r.first.second);
        size_t rRUCx = findCIndex(cx, r.second.first);
        size_t rRUCy = findCIndex(cy, r.second.second);
        for (uint64_t i = rLFCx; i < rRUCx + 1; ++i) {
            for (uint64_t j = rLFCy; j < rRUCy + 1; ++j) {
                benchmark::DoNotOptimize(++map[i][j]);
            }
        }
    }
    return {map, cx, cy};
}

static void MAP_BUILD_BENCH(benchmark::State &state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto rects = generateRects(static_cast<uint64_t>(state.range(0)));
        state.ResumeTiming();
        benchmark::DoNotOptimize(std::get<0>(build_map(rects)).size());
    }
}

BENCHMARK(MAP_BUILD_BENCH)->ArgName("N")->RangeMultiplier(2)->Range(8, 2048);

static void MAP_QUERY_BENCH(benchmark::State &state) {
    auto rects = generateRects(static_cast<uint64_t>(state.range(0)));
    auto [map, cx, cy] = build_map(rects);
    auto points = generatePoints(static_cast<uint64_t>(state.range(0)));
    std::size_t currentIdx = 0;

    for (auto _ : state) {
        uint64_t answer = 0;
        const auto &p = points[currentIdx];
        if (p.first >= cx.front() && p.second >= cy.front()) {
            auto cPx = findCIndex(cx, p.first);
            auto cPy = findCIndex(cy, p.second);
            benchmark::DoNotOptimize(answer += map[cPx][cPy]);
        }
        currentIdx += 1;
        currentIdx %= points.size();
    }
}

BENCHMARK(MAP_QUERY_BENCH)
    ->ArgName("N")
    ->RangeMultiplier(2)
    ->Range(8, 2048)
    ->Iterations(10000);
#endif  // MAP_BENCHES_HPP
