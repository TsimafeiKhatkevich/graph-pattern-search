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

static const ui32 NOT_A_MARK = ui32(-1);

void DFS(const ui32 vertex, const ui32 curMark, std::vector<ui32>& allMarks, const TAdjMatrix& graph) {
    allMarks[vertex] = curMark;
    for (auto neighbour : graph[vertex]) {
        if (allMarks[neighbour] == NOT_A_MARK) {
            DFS(neighbour, curMark, allMarks, graph);
        }
    }
}

std::vector<ui32> FindConnectedComponents(const TAdjMatrix& graph) {
    std::vector<ui32> marks(graph.size(), NOT_A_MARK);
    ui32 maxMark = 0;
    for (ui32 i = 0; i < marks.size(); ++i) {
        if (marks[i] == NOT_A_MARK) {
            DFS(i, maxMark, marks, graph);
            ++maxMark;
        }
    }
    return marks;
}

