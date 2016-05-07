#pragma once

#include "common.h"
#include <random>
#include <memory>

class TERModelGenerator {
public:
#ifdef RND_DEBUG
    using TRandGenerator = std::mt19937;
#else
    using TRandGenerator = std::random_device;
#endif

private:
    bool IsDirected = false;
    bool IsFast = true;
    std::shared_ptr<TAdjMatrix> LastResultPtr;
    TRandGenerator RndGen;

private:
    inline bool TryInsertEdge(const ui32 src, const ui32 dst) {
        auto& srcList = (*LastResultPtr)[src];
        if (srcList.count(dst)) {
            return false;
        } else {
            srcList.insert(dst);
            return true;
        }
    }

    void MakeSlow(const ui32 n, const double p);
    void MakeFast(const ui32 nVertices, const ui32 nEdges);

public:
    TERModelGenerator(bool directed = false, bool fast = true)
        : IsDirected(directed),
        IsFast(fast)
    {
#ifdef RND_DEBUG
        RndGen.discard(1);
#endif
    }

    std::shared_ptr<TAdjMatrix> operator ()(const ui32 n, const double p);
 };
