#pragma once

#include <unordered_set>
#include <vector>
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
