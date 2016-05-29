#include "search_bf.h"

#include <stack>

struct TBFState {
    ui32 PInd;
    std::vector<ui32>::const_iterator MatchIt;
    std::vector<ui32>::const_iterator EndIt;
};

bool TBFSearchProcessor::DoSearch(const TAdjMatrix& pattern, std::vector<char> &vFlags) {
    const auto patternEdges = FromTAdjMatrix<TEdgeListIndexed>(pattern);
    std::vector<ui32> allVertices;
    for (ui32 i = 0; i < vFlags.size(); ++i) {
        if (vFlags[i] == FLAG_AVAILABLE) {
            allVertices.push_back(i);
        }
    }
    static const ui32 undefined = ui32(-1);
    const ui32 pNodes = ui32(pattern.size());

    TMatch match(pNodes, undefined);

    std::stack<TBFState> bfState;
    bfState.push({0, allVertices.begin(), allVertices.end()});
    while (true) {
        auto& cur = bfState.top();
        auto& cMatchIt = cur.MatchIt;
        while (cMatchIt != cur.EndIt &&
                (patternEdges[cur.PInd].size() > Graph[*cMatchIt].size() ||
                vFlags[*cMatchIt] != FLAG_AVAILABLE)) {
            ++cMatchIt;
        }

        if (cMatchIt == cur.EndIt) {
            bfState.pop();
            if (bfState.empty()) {
                return !Results.empty();
            }
            auto& prev = bfState.top();
            vFlags[*prev.MatchIt] = FLAG_AVAILABLE;
            match[prev.PInd] = undefined;
            ++prev.MatchIt;
            continue;
        }

        const auto cMatchVal = *cMatchIt;
        match[cur.PInd] = cMatchVal;
        vFlags[cMatchVal] = FLAG_IN_PATTERN;

        ui32 vNext = undefined;
        for (auto v : patternEdges[cur.PInd]) {
            if (match[v] == undefined) {
                vNext = v;
                break;
            }
        }
        if (vNext != undefined) {
            bfState.push({vNext, Graph[cMatchVal].begin(), Graph[cMatchVal].end()});
            continue;
        }

        for (ui32 v = 0; v < pNodes; ++v) {
            if (match[v] == undefined) {
                vNext = v;
                break;
            }
        }
        if (vNext != undefined) {
            bfState.push({vNext, allVertices.begin(), allVertices.end()});
            continue;
        }

        // now check if this is really our pattern
        bool isFound = true;
        for (ui32 i = 0; i < pNodes && isFound; ++i) {
            ui32 iMatch = match[i];
            for (ui32 j : patternEdges[i]) {
                if (!GraphFast.IsEdge(iMatch, match[j])) {
                    isFound = false;
                    break;
                }
            }
        }
        if (isFound && SaveOnlyInduced) {
            isFound = IsInducedPattern(*HostGraph, pattern, match, vFlags);
        }
        if (isFound) {
            Results.push_back(match);
            if (StopOnFirst) {
                return true;
            }
        }
        vFlags[cMatchVal] = FLAG_AVAILABLE;
        match[cur.PInd] = undefined;
        ++cMatchIt;
    }
}
