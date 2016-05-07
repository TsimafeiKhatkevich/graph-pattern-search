#include "common.h"
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
}

int main() {
    TERModelGenerator gGenerator;
    const auto graph = gGenerator(N, P);
    TAdjMatrixFast adjFast(*graph);
    PrintGraph(std::cout, adjFast);

    std::ifstream fin("pattern.in");
    const auto pattern = ReadPattern(fin, IS_DIRECTED);

    return 0;
}
