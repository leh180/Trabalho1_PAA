#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>

#include "DataStructure.hpp"

// Região 2D delimitada por (R,G)
struct AABB2D {
    double minR, maxR;
    double minG, maxG;

    AABB2D() : minR(0), maxR(0), minG(0), maxG(0) {}
    AABB2D(double r0, double r1, double g0, double g1)
        : minR(r0), maxR(r1), minG(g0), maxG(g1) {}

    double minDistRG(double rq, double gq) const {
        double dr = 0.0;
        if (rq < minR) dr = minR - rq;
        else if (rq > maxR) dr = rq - maxR;

        double dg = 0.0;
        if (gq < minG) dg = minG - gq;
        else if (gq > maxG) dg = gq - maxG;

        return std::sqrt(dr*dr + dg*dg);
    }

    bool contains(double r, double g) const {
        return (r >= minR && r <= maxR && g >= minG && g <= maxG);
    }

    double midR() const { return (minR + maxR) * 0.5; }
    double midG() const { return (minG + maxG) * 0.5; }
};

// Nó da árvore (até 4 filhos)
class QuadNode {
public:
    static constexpr int CAPACITY = 8;

    AABB2D bbox;
    std::vector<FeatureVector> pts;
    std::array<std::unique_ptr<QuadNode>, 4> child;
    bool isLeaf;

    QuadNode(const AABB2D& box) : bbox(box), isLeaf(true) {}

    void subdivide() {
        if (!isLeaf) return;
        double rMid = bbox.midR();
        double gMid = bbox.midG();

        child[0] = std::make_unique<QuadNode>(AABB2D(bbox.minR, rMid, gMid,     bbox.maxG)); // NW
        child[1] = std::make_unique<QuadNode>(AABB2D(rMid,     bbox.maxR, gMid, bbox.maxG)); // NE
        child[2] = std::make_unique<QuadNode>(AABB2D(bbox.minR, rMid,     bbox.minG, gMid)); // SW
        child[3] = std::make_unique<QuadNode>(AABB2D(rMid,     bbox.maxR, bbox.minG, gMid)); // SE
        isLeaf = false;
    }

    static int quadrantOf(double r, double g, double rMid, double gMid) {
        if (g >= gMid) {
            return (r <= rMid) ? 0 : 1;
        } else {
            return (r <= rMid) ? 2 : 3;
        }
    }
};

// Estrutura principal
class Quadtree : public DataStructure {
private:
    std::unique_ptr<QuadNode> root;

public:
    Quadtree(double rMin = 0.0, double rMax = 255.0,
             double gMin = 0.0, double gMax = 255.0) {
        root = std::make_unique<QuadNode>(AABB2D(rMin, rMax, gMin, gMax));
    }

    ~Quadtree() override = default;

    void insert(const FeatureVector& vec) override {
        ensureRootContains(vec.r, vec.g);
        insertRec(root.get(), vec);
    }

    // Busca k-vizinhos mais próximos
    QueryResult query(const FeatureVector& query_vec, int k) override {
        QueryResult result;
        if (k <= 0) return result;

        using Pair = std::pair<double, FeatureVector>;
        auto cmpMaxHeap = [](const Pair& a, const Pair& b){ return a.first < b.first; };
        std::priority_queue<Pair, std::vector<Pair>, decltype(cmpMaxHeap)> best(cmpMaxHeap);

        struct PQNode {
            double bound;
            QuadNode* node;
            bool operator>(const PQNode& other) const { return bound > other.bound; }
        };
        std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> fringe;

        fringe.push(PQNode{ root->bbox.minDistRG(query_vec.r, query_vec.g), root.get() });

        double worstBest = std::numeric_limits<double>::infinity();

        while (!fringe.empty()) {
            PQNode cur = fringe.top(); fringe.pop();

            if (best.size() == static_cast<size_t>(k) && cur.bound >= worstBest) break;

            QuadNode* node = cur.node;
            if (node->isLeaf) {
                for (const auto& p : node->pts) {
                    double dist = query_vec.distanceTo(p);
                    result.comparisons++;
                    if (best.size() < static_cast<size_t>(k)) {
                        best.emplace(dist, p);
                        if (best.size() == static_cast<size_t>(k)) worstBest = best.top().first;
                    } else if (dist < best.top().first) {
                        best.pop();
                        best.emplace(dist, p);
                        worstBest = best.top().first;
                    }
                }
            } else {
                for (int q = 0; q < 4; ++q) {
                    QuadNode* ch = node->child[q].get();
                    if (!ch) continue;
                    double b = ch->bbox.minDistRG(query_vec.r, query_vec.g);
                    if (best.size() == static_cast<size_t>(k) && b >= worstBest) continue;
                    fringe.push(PQNode{ b, ch });
                }
            }
        }

        std::vector<Pair> tmp;
        tmp.reserve(best.size());
        while (!best.empty()) { tmp.push_back(best.top()); best.pop(); }
        std::sort(tmp.begin(), tmp.end(),
                  [](const Pair& a, const Pair& b){ return a.first < b.first; });

        result.neighbors.reserve(tmp.size());
        for (auto& pr : tmp) result.neighbors.push_back(pr.second);
        return result;
    }

private:
    // Inserção recursiva
    void insertRec(QuadNode* node, const FeatureVector& vec) {
        if (node->isLeaf) {
            node->pts.push_back(vec);
            if ((int)node->pts.size() > QuadNode::CAPACITY) {
                std::vector<FeatureVector> oldPts;
                oldPts.swap(node->pts);
                node->subdivide();
                for (const auto& p : oldPts) insertIntoChild(node, p);
            }
        } else {
            insertIntoChild(node, vec);
        }
    }

    void insertIntoChild(QuadNode* node, const FeatureVector& vec) {
        double rMid = node->bbox.midR();
        double gMid = node->bbox.midG();
        int q = QuadNode::quadrantOf(vec.r, vec.g, rMid, gMid);
        QuadNode* ch = node->child[q].get();
        insertRec(ch, vec);
    }

    // Expande a raiz para incluir pontos fora do box atual
    void ensureRootContains(double r, double g) {
        if (root->bbox.contains(r, g)) return;

        while (!root->bbox.contains(r, g)) {
            AABB2D oldBox = root->bbox;
            AABB2D newBox = oldBox;

            if (r < oldBox.minR) newBox.minR = oldBox.minR - (oldBox.maxR - oldBox.minR);
            else if (r > oldBox.maxR) newBox.maxR = oldBox.maxR + (oldBox.maxR - oldBox.minR);

            if (g < oldBox.minG) newBox.minG = oldBox.minG - (oldBox.maxG - oldBox.minG);
            else if (g > oldBox.maxG) newBox.maxG = oldBox.maxG + (oldBox.maxG - oldBox.minG);

            auto newRoot = std::make_unique<QuadNode>(newBox);
            newRoot->subdivide();

            int qOld = QuadNode::quadrantOf((oldBox.minR + oldBox.maxR)*0.5,
                                            (oldBox.minG + oldBox.maxG)*0.5,
                                            newRoot->bbox.midR(),
                                            newRoot->bbox.midG());
            newRoot->child[qOld] = std::move(root);
            newRoot->isLeaf = false;

            root = std::move(newRoot);
        }
    }
};

#endif // QUADTREE_HPP
