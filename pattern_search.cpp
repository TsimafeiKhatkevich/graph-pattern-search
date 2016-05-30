#include "pattern_search.h"

#include <sstream>
#include <algorithm>

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

TCanonicalSets CanonizeMatchedCycles(
        const TAdjMatrix& hostGraph,
        const TAdjMatrix& pattern,
        const std::vector<TSearchProcessorBase::TMatch>& matches) {
    TCanonicalSets result;
    for (auto match : matches) {
        // eliminate offset
        auto minIt = match.begin();
        ui32 min = *minIt;
        for (auto it = minIt; it != match.end(); ++it) {
            if (*it < min) {
                minIt = it;
                min = *it;
            }
        }
        if (minIt != match.begin()) {
            std::reverse(match.begin(), minIt);
            std::reverse(minIt, match.end());
            std::reverse(match.begin(), match.end());
        }
        // eliminate symmetry
        if (match.back() < match[1]) {
            std::reverse(match.begin() + 1, match.end());
        }
        // to string
        std::ostringstream strout;
        for (const auto i : match) {
            strout << i << " ";
        }
        const std::string& cycleStr = strout.str();

        if (result.All.count(cycleStr)) {
            continue;
        }
        result.All.insert(cycleStr);
        // check for induced
        std::vector<char> vFlags(hostGraph.size(), FLAG_AVAILABLE);
        for (const auto& m : match) {
            vFlags[m] = FLAG_IN_PATTERN;
        }
        if (IsInducedPattern(hostGraph, pattern, match, vFlags)) {
            result.Induced.insert(cycleStr);
        }
    }
    return result;
}
