#include "pattern_search.h"

std::vector<char> GetReasonableVertices(const TAdjMatrix& hostGraph, const TAdjMatrix& pattern) {
    const auto pComps = FindConnectedComponents(pattern);
    std::unordered_map<ui32, ui32> pCompSizes;
    for (ui32 mark : pComps) {
        ++pCompSizes[mark];
    }
    ui32 minSize = pattern.size();
    for (auto mark2size : pCompSizes) {
        if (mark2size.second < minSize) {
            minSize = mark2size.second;
        }
    }

    std::vector<char> result(hostGraph.size(), FLAG_NOT_REASONABLE);
    const auto hComps = FindConnectedComponents(hostGraph);
    std::unordered_map<ui32, ui32> hCompSizes;
    for (ui32 mark : hComps) {
        ++hCompSizes[mark];
    }

    for (ui32 vertex = 0; vertex < result.size(); ++vertex) {
        const ui32 compId = hComps[vertex];
        if (hCompSizes[compId] >= minSize) {
            result[vertex] = FLAG_AVAILABLE;
        }
    }
    return result;
}

