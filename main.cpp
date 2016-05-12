#include "pattern_search.h"
#include "er_model.h"

#include <fstream>

static const ui32 N = 10;
static const double P = 0.2;
static const bool IS_DIRECTED = false;

TAdjMatrix ReadPattern(std::istream& in, bool isDirected) {
    TAdjMatrix result;
    ui32 nVertices;
    ui32 nEdges;
    in >> nVertices >> nEdges;
    result.resize(nVertices);
    for (ui32 i = 0; i < nEdges; ++i) {
        ui32 src;
        ui32 dst;
        in >> src >> dst;
        if (src < nVertices && dst < nVertices) {
            result[src].insert(dst);
            if (!isDirected) {
                result[dst].insert(src);
            }
        }
    }
    return result;
}

void OutputForPlot(std::ostream& out, const std::vector<ui32>& match) {
    for (size_t i = 0; i + 1 < match.size(); ++i) {
        out << match[i] << " ";
    }
    out << match.back() << std::endl;
}

int main() {
    TERModelGenerator gGenerator;
    const auto graph = gGenerator(N, P);
    TAdjMatrixFast adjFast(*graph);
    std::ofstream fout("model.out");
    PrintGraph(fout, adjFast);
    fout.close();
    const auto comp = FindConnectedComponents(*graph);
    for (auto mark : comp) {
        std::cout << mark << " ";
    }
    std::cout << std::endl;

    std::ifstream fin("pattern.in");
    const auto pattern = ReadPattern(fin, IS_DIRECTED);
    PrintGraph(std::cout, pattern);

    const auto reas = GetReasonableVertices(*graph, pattern);
    std::cout << "reasonable: ";
    for (auto mark : reas) {
        std::cout << ui32(mark) << " ";
    }
    std::cout << std::endl;

    return 0;
}
