#ifndef SEGTREE_BENCHES_HPP
#define SEGTREE_BENCHES_HPP

#include "utils.hpp"
#include "benchmark/benchmark.h"
#include "generators.hpp"
#include <algorithm>
#include <memory>

struct Node {
    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;
    int64_t sum = 0;
};

struct E {
    size_t cx;
    bool f;
    size_t bCy;
    size_t eCy;

    E(size_t cx_, bool f_, size_t bCy_, size_t eCy_)
        : cx(cx_), f(f_), bCy(bCy_), eCy(eCy_) {
    }
};

auto modTree(std::shared_ptr<Node> root, size_t l, size_t r, size_t tl,
             size_t tr, int64_t adj) -> std::shared_ptr<Node> {
    if (tl >= r || l >= tr) {
        return root;
    }
    if (tl <= l && r <= tr) {
        std::shared_ptr<Node> newRoot(new Node(*root));
        newRoot->sum += adj;
        return newRoot;
    }
    size_t middle = l + (r - l) / 2;
    std::shared_ptr<Node> newRoot(new Node(*root));
    if (!newRoot->left)
        newRoot->left = std::shared_ptr<Node>(new Node);
    if (!newRoot->right)
        newRoot->right = std::shared_ptr<Node>(new Node);
    newRoot->left = modTree(newRoot->left, l, middle, tl, tr, adj);
    newRoot->right = modTree(newRoot->right, middle, r, tl, tr, adj);
    return newRoot;
}

auto build_segtree(std::vector<std::pair<std::pair<int64_t, int64_t>,
                                         std::pair<int64_t, int64_t>>>& rects)
    -> std::tuple<std::vector<std::shared_ptr<Node>>, std::vector<size_t>,
                  std::vector<int64_t>, std::vector<int64_t>> {
    std::vector<int64_t> cx;
    std::vector<int64_t> cy;
    for (const auto& r : rects) {
        cx.push_back(r.first.first);
        cx.push_back(r.second.first);
        cx.push_back(r.second.first + 1);
        cy.push_back(r.first.second);
        cy.push_back(r.second.second);
        cy.push_back(r.second.second + 1);
    }
    compress(cx);
    compress(cy);

    std::vector<E> es;
    for (const auto& r : rects) {
        es.emplace_back(findCIndex(cx, r.first.first), true,
                        findCIndex(cy, r.first.second),
                        findCIndex(cy, r.second.second) + 1);
        es.emplace_back(findCIndex(cx, r.second.first) + 1, false,
                        findCIndex(cy, r.first.second),
                        findCIndex(cy, r.second.second) + 1);
    }
    std::sort(es.begin(), es.end(), [](const auto& e1, const auto& e2) -> bool {
        return e1.cx < e2.cx;
    });

    std::shared_ptr<Node> root(new Node);
    std::vector<size_t> rootsCx;
    std::vector<std::shared_ptr<Node>> roots;

    size_t lastCx = es.front().cx;
    for (const auto& e : es) {
        if (e.cx > lastCx) {
            roots.push_back(root);
            rootsCx.push_back(lastCx);
            lastCx = e.cx;
        }
        root = modTree(root, 0, cy.size(), e.bCy, e.eCy, e.f ? 1 : -1);
    }
    roots.push_back(root);
    rootsCx.push_back(lastCx);

    return {roots, rootsCx, cx, cy};
}

auto calc(std::shared_ptr<Node> root, size_t l, size_t r, size_t target)
    -> int64_t {
    if (r - l == 1u) {
        return root->sum;
    }
    size_t middle = l + (r - l) / 2;
    return root->sum +
           (target < middle
                ? root->left ? calc(root->left, l, middle, target) : 0
            : root->right ? calc(root->right, middle, r, target)
                          : 0);
}

static void SEGTREE_BUILD_BENCH(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto rects = generateRects(static_cast<uint64_t>(state.range(0)));
        state.ResumeTiming();
        benchmark::DoNotOptimize(std::get<0>(build_segtree(rects)).size());
    }
}

BENCHMARK(SEGTREE_BUILD_BENCH)
    ->ArgName("N")
    ->RangeMultiplier(2)
    ->Range(8, 100000);

static void SEGTREE_QUERY_BENCH(benchmark::State& state) {
    auto rects = generateRects(static_cast<uint64_t>(state.range(0)));
    auto [roots, rootsCx, cx, cy] = build_segtree(rects);
    auto points = generatePoints(static_cast<uint64_t>(state.range(0)));
    std::size_t currentIdx = 0;

    for (auto _ : state) {
        int64_t answer = 0;
        const auto& p = points[currentIdx];
        if (p.first >= cx.front() && p.second >= cy.front()) {
            auto cPx = findCIndex(cx, p.first);
            auto cPy = findCIndex(cy, p.second);
            auto rootCx = findCIndex(rootsCx, cPx);
            auto root = roots[rootCx];
            benchmark::DoNotOptimize(answer += calc(root, 0, cy.size(), cPy));
        }
        currentIdx += 1;
        currentIdx %= points.size();
    }
}

BENCHMARK(SEGTREE_QUERY_BENCH)
    ->ArgName("N")
    ->RangeMultiplier(2)
    ->Range(8, 100000);
#endif  // SEGTREE_BENCHES_HPP
