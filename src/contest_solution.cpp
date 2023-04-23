#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

typedef std::pair<int64_t, int64_t> point;
typedef std::pair<point, point> rect;

template <typename T>
void compress(std::vector<T>& vec) {
  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template <typename T>
auto findCIndex(std::vector<T>& vec, T value) -> size_t {
  return static_cast<size_t>(std::upper_bound(vec.begin(), vec.end(), value) -
                             vec.begin() - 1u);
}

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
      : cx(cx_), f(f_), bCy(bCy_), eCy(eCy_) {}
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
  if (!newRoot->left) newRoot->left = std::shared_ptr<Node>(new Node);
  if (!newRoot->right) newRoot->right = std::shared_ptr<Node>(new Node);
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
  return root->sum + (target < middle
                          ? root->left ? calc(root->left, l, middle, target) : 0
                      : root->right ? calc(root->right, middle, r, target)
                                    : 0);
}

int main() {
  std::cin.tie(0);
  std::ios_base::sync_with_stdio(0);

  uint32_t rectsNumber;
  std::cin >> rectsNumber;
  
  if (rectsNumber == 0) {
    uint32_t pointsNumber;
    std::cin >> pointsNumber;

    while (pointsNumber--) {
      std::cout << 0 << ' ';
    }
    return 0;
  }

  std::vector<rect> rects;
  for (uint32_t i = 0; i < rectsNumber; ++i) {
    point a;
    point b;
    std::cin >> a.first >> a.second >> b.first >> b.second;
    rects.push_back({a, b});
  }

  auto [roots, rootsCx, cx, cy] = build_segtree(rects);

  uint32_t pointsNumber;
  std::cin >> pointsNumber;

  while (pointsNumber--) {
    int64_t x, y;
    std::cin >> x >> y;
    if (rectsNumber > 0) {
      if (x >= cx.front() && y >= cy.front()) {
        auto cPx = findCIndex(cx, x);
        auto cPy = findCIndex(cy, y);
        auto rootCx = findCIndex(rootsCx, cPx);
        auto root = roots[rootCx];
        std::cout << calc(root, 0, cy.size(), cPy) << ' ';
      } else {
        std::cout << 0 << ' ';
      }
    } else {
      std::cout << 0 << ' ';
    }
  }
}
