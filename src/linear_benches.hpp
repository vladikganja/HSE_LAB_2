#ifndef LINEAR_BENCHES_HPP
#define LINEAR_BENCHES_HPP

#include "benchmark/benchmark.h"
#include "generators.hpp"

static void LINEAR_BUILD_BENCH(benchmark::State &state) {
    auto rectangles = generateRects(static_cast<uint64_t>(state.range(0)));
    for (auto _ : state) {
        // Ничего нет, так как линейный алгоритм не требует построения
        // структуры.
    }
}

BENCHMARK(LINEAR_BUILD_BENCH)
    ->ArgName("N")
    ->RangeMultiplier(2)
    ->Range(8, 100000)
    ->Iterations(10000);

static void LINEAR_QUERY_BENCH(benchmark::State &state) {
    auto rectangles = generateRects(static_cast<uint64_t>(state.range(0)));
    auto points = generatePoints(static_cast<uint64_t>(state.range(0)));
    std::size_t currentIdx = 0;
    for (auto _ : state) {
        uint64_t answer = 0;
        const auto &p = points[currentIdx];
        for (const auto &r : rectangles) {
            benchmark::DoNotOptimize(answer += (r.first.first <= p.first &&
                                                p.first <= r.second.first &&
                                                r.first.second <= p.second &&
                                                p.second <= r.second.second));
        }
        currentIdx += 1;
        currentIdx %= points.size();
    }
}

BENCHMARK(LINEAR_QUERY_BENCH)
    ->ArgName("N")
    ->RangeMultiplier(2)
    ->Range(8, 100000)
    ->Iterations(10000);
#endif  // LINEAR_BENCHES_HPP
