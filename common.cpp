#include "common.h"

void PrintGraph(std::ostream& out, const TAdjMatrix& graph, bool ignoreDirection) {
    for (size_t i = 0; i < graph.size(); ++i) {
        const auto& neighbors = graph[i];
        if (neighbors.empty()) {
            continue;
        }
        std::vector<ui32> sorted;
        sorted.reserve(neighbors.size());
        for (const auto v : neighbors) {
            sorted.push_back(v);
        }
        std::sort(sorted.begin(), sorted.end());

        for (const auto v : sorted) {
            if (ignoreDirection && i > v) {
                continue;
            }
            out << i << "\t" << v << std::endl;
        }
    }
}

void PrintGraph(std::ostream& out, const TAdjMatrixFast& graph, bool ignoreDirection) {
    for (size_t i = 0; i < graph.Size(); ++i) {
        const size_t lowBound = ignoreDirection ? i : 0;
        for (size_t j = lowBound; j < graph.Size(); ++j) {
            if (graph.IsEdge(i, j)) {
                out << i << "\t" << j << std::endl;
            }
        }
    }
}

