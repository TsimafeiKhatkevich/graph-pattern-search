#pragma once

#include <unordered_set>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

#define UNUSED(F) \
    do { (void)(F); } while (0)

//#define RND_DEBUG

using ui32 = uint32_t;
using ui64 = uint64_t;

using TEdge = std::pair<ui32, ui32>;
/**
 * @brief Just list of edges
 */
using TEdgeList = std::vector<TEdge>;

/**
 * @brief TAdjMatrix is used when you need to iterate over neighbours
 * but edge existence check is more often action
 */
using TAdjMatrix = std::vector<std::unordered_set<ui32> >;

/**
 * @brief TEdgeListIndexed - is a sparse variant of adjacency matrix
 * which is needed only if you need a lot of iterations over nodes neighbours
 */
using TEdgeListIndexed = std::vector<std::vector<ui32> >;

/**
 * @brief The fastest variant for edge existing checks
 */
class TAdjMatrixFast {
private:
    const size_t BaseSize;
    std::vector<bool> Data;

public:
    TAdjMatrixFast(const size_t size)
        : BaseSize(size),
        Data(size*size, false)
    {
    }

    TAdjMatrixFast(const TAdjMatrix& graph)
        : TAdjMatrixFast(graph.size())
    {
        for (size_t i = 0; i < graph.size(); ++i) {
            for (const auto v : graph[i]) {
                this->Set(i, v, true);
            }
        }
    }

    const std::vector<bool>& GetData() const {
        return Data;
    }

    size_t Size() const {
        return BaseSize;
    }

    void Set(const ui32 i, const ui32 j, const bool bit) {
        Data[i * BaseSize + j] = bit;
    }

    bool IsEdge(const ui32 i, const ui32 j) const {
        return Data[i * BaseSize + j];
    }

};

template<typename T>
inline T FromTAdjMatrix(const TAdjMatrix& /*graph*/) {
    throw std::exception("FromTAdjMatrix : unavailable type conversion\n");
}

template<>
inline TAdjMatrixFast FromTAdjMatrix<TAdjMatrixFast>(const TAdjMatrix& graph) {
    return TAdjMatrixFast(graph);
}

template<>
inline TEdgeListIndexed FromTAdjMatrix<TEdgeListIndexed>(const TAdjMatrix& graph) {
    TEdgeListIndexed result(graph.size());
    for (size_t i = 0; i < graph.size(); ++i) {
        result[i].reserve(graph[i].size());
        for (auto v : graph[i]) {
            result[i].push_back(v);
        }
    }
    return result;
}

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
public:
    using TMatch = std::vector<ui32>;

protected:
    std::shared_ptr<TAdjMatrix> HostGraph;
    bool StopOnFirst;
    bool SaveOnlyInduced;

    std::vector<TMatch> Results;

public:
    TSearchProcessorBase(std::shared_ptr<TAdjMatrix> graph, bool stopOnFirst, bool saveOnlyInduced)
        : HostGraph(graph)
        , StopOnFirst(stopOnFirst)
        , SaveOnlyInduced(saveOnlyInduced)
    {
    }

    virtual bool DoSearch(const TAdjMatrix& pattern, std::vector<char>& vFlags) = 0;

    virtual bool DoSearch(const TAdjMatrix &pattern) {
        std::vector<char> vFlags(HostGraph->size(), FLAG_AVAILABLE);
        return DoSearch(pattern, vFlags);
    }

    const std::vector<TMatch>& GetResults() const {
        return Results;
    }

    void ClearResults() {
        Results.clear();
    }
};

