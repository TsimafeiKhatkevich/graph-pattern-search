#pragma once

#include <unordered_set>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

#define UNUSED(F) \
    do { (void)(F); } while (0)

#define RND_DEBUG

using ui32 = uint32_t;
using ui64 = uint64_t;

using TEdge = std::pair<ui32, ui32>;
using TEdgeList = std::vector<TEdge>;
using TAdjMatrix = std::vector<std::unordered_set<ui32> >;

class TAdjMatrixFast {
private:
    const size_t BaseSize;
    std::vector<char> Data;

public:
    TAdjMatrixFast(const size_t size)
        : BaseSize(size),
        Data(size*size, 0)
    {
    }

    TAdjMatrixFast(const TAdjMatrix& graph)
        : TAdjMatrixFast(graph.size())
    {
        for (size_t i = 0; i < graph.size(); ++i) {
            for (const auto v : graph[i]) {
                (*this)(i, v) = 1;
            }
        }
    }

    size_t Size() const {
        return BaseSize;
    }

    char& operator ()(const ui32 i, const ui32 j) {
        return Data[i * BaseSize + j];
    }

    const char& operator ()(const ui32 i, const ui32 j) const {
        return Data[i * BaseSize + j];
    }

    bool IsEdge(const ui32 i, const ui32 j) const {
        return Data[i * BaseSize + j] > 0;
    }

};

void PrintGraph(std::ostream& out, const TAdjMatrix& graph, bool ignoreDirection = true);

void PrintGraph(std::ostream& out, const TAdjMatrixFast& graph, bool ignoreDirection = true);

std::vector<ui32> FindConnectedComponents(const TAdjMatrix& graph);

static const char FLAG_NOT_REASONABLE = 0;
static const char FLAG_AVAILABLE = 1;
static const char FLAG_IN_PATTERN = 2;

inline bool IsInducedPattern(
        const TAdjMatrix& hostGraph,
        const TAdjMatrix& pattern,
        const std::vector<ui32>& mapping,
        const std::vector<char>& vFlags) {
    for (size_t pVertex = 0; pVertex < mapping.size(); ++pVertex) {
        const size_t patternDegree = pattern[pVertex].size();
        size_t hostDegree = 0;
        for (ui32 hVertex : hostGraph[mapping[pVertex]]) {
            if (vFlags[hVertex] == FLAG_IN_PATTERN) {
                ++hostDegree;
            }
        }
        if (hostDegree > patternDegree) {
            return false;
        }
    }
    return true;
}

class TSearchProcessorBase {
protected:
    bool SearchForInduced;
    std::shared_ptr<TAdjMatrix> HostGraph;

public:
    struct TResult {
        std::vector<ui32> Match;
        bool PatternFound = false;
        bool MatchIsMapping = false;
    };

    TSearchProcessorBase(bool searchForInduced, std::shared_ptr<TAdjMatrix> graph)
        : SearchForInduced(searchForInduced)
        , HostGraph(graph)
    {
    }

    virtual TResult Find(const TAdjMatrix& pattern, std::vector<char>& vFlags) = 0;

    virtual TResult Find(const TAdjMatrix &pattern) {
        std::vector<char> vFlags(HostGraph->size(), FLAG_AVAILABLE);
        return Find(pattern, vFlags);
    }
};

