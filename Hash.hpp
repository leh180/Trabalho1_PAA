#ifndef HASH_HPP
#define HASH_HPP

#include <vector>
#include <unordered_set>
#include <algorithm>

#include "DataStructure.hpp"
#include "Vector.hpp"

struct HashNode {
    FeatureVector data;
    HashNode* next;

    HashNode() {
        next = nullptr;
    }

    HashNode(const FeatureVector& v) {
        data = v;
        next = nullptr;
    }
};

class HashTable : public DataStructure {
private:
    int numBuckets;
    int numHashes;
    int binSize;
    std::vector<std::vector<HashNode*>> tables;
    int comparisons;

    int hashFunction(const FeatureVector& vec, int seed) const {
        int r_bin = static_cast<int>((vec.r + seed) / binSize);
        int g_bin = static_cast<int>((vec.g + seed) / binSize);
        int b_bin = static_cast<int>((vec.b + seed) / binSize);
        int hash_value = (r_bin * 73856093) ^ (g_bin * 19349663) ^ (b_bin * 83492791) ^ seed; //Espalhar e reduzir colisões
        return (hash_value % numBuckets + numBuckets) % numBuckets; //garantir que seja positivo
    }

public:
    HashTable(int buckets = 1013, int hashes = 5, int bin = 25)
        : numBuckets(buckets), numHashes(hashes), binSize(bin), comparisons(0){
        tables.resize(numHashes, std::vector<HashNode*>(numBuckets, nullptr));
    }

    ~HashTable() {
        for (int h = 0; h < numHashes; h++) {
            for (auto head : tables[h]) {
                while (head) {
                    HashNode* tmp = head;
                    head = head->next;
                    delete tmp;
                }
            }
        }
    }

    void insert(const FeatureVector& vec) override {
        for (int h = 0; h < numHashes; h++) {
            int idx = hashFunction(vec, h);
            HashNode* node = new HashNode(vec);
            node->next = tables[h][idx];
            tables[h][idx] = node;
        }
    }

    QueryResult query(const FeatureVector& q, int k) override {
        QueryResult result;
        std::vector<FeatureVector> candidates;
        std::unordered_set<FeatureVector*> seen;

        for (int h = 0; h < numHashes; h++) {
            int idx = hashFunction(q, h);
            HashNode* node = tables[h][idx];
            while (node) {
                if (seen.find(&node->data) == seen.end()) {
                    candidates.push_back(node->data);
                    seen.insert(&node->data);
                }
                node = node->next;
            }
        }
        comparisons = candidates.size();

        std::sort(candidates.begin(), candidates.end(), [&](const FeatureVector& a, const FeatureVector& b){
            return q.similarityTo(a) > q.similarityTo(b);
        });

        for (int i = 0; i < std::min(k, (int)candidates.size()); i++) {
            result.neighbors.push_back(candidates[i]);
        }

        result.comparisons = comparisons;
        return result;
    }
};

#endif
