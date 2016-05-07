#include "er_model.h"

void TERModelGenerator::MakeSlow(const ui32 n, const double p) {
    std::bernoulli_distribution edgeSampler(p);
    for (ui32 i = 0; i < n; ++i) {
        for (ui32 j = i + 1; j < n; ++j) {
            edgeSampler.reset();
            const bool toDst = edgeSampler(RndGen);
            edgeSampler.reset();
            const bool toSrc = IsDirected ? edgeSampler(RndGen) : toDst;
            if (toDst) {
                TryInsertEdge(i, j);
            }
            if (toSrc) {
                TryInsertEdge(j, i);
            }
        }
    }
}

void TERModelGenerator::MakeFast(const ui32 nVertices, const ui32 nEdges) {
    std::uniform_int_distribution<ui32> vSampler(0, nVertices - 1);
    ui32 totalEdges = 0;
    while (totalEdges < nEdges) {
        vSampler.reset();
        const ui32 src = vSampler(RndGen);
        vSampler.reset();
        const ui32 dst = vSampler(RndGen);
        if (src != dst && TryInsertEdge(src, dst)) {
            ++totalEdges;
            if (!IsDirected) {
                TryInsertEdge(dst, src);
            }
        }
    }
}

std::shared_ptr<TAdjMatrix> TERModelGenerator::operator ()(const ui32 n, const double p) {
    LastResultPtr.reset(new TAdjMatrix(n));
    if (IsFast) {
        const double edgesMean = n * (n - 1) * p * (IsDirected ? 1.0 : 0.5);
        const double edgesStdev = std::sqrt(edgesMean * (1.0 - p));
        std::normal_distribution<double> edgesDist(edgesMean, edgesStdev);
        const ui32 nEdges = static_cast<ui32>(std::max(0.0, edgesDist(RndGen)));
        MakeFast(n, nEdges);
    } else {
        MakeSlow(n, p);
    }
    return LastResultPtr;
}
