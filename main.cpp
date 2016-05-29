#include "pattern_search.h"
#include "er_model.h"

#include <fstream>

static const ui32 N = 20;
static const double P = 0.15;
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
    {
        std::ofstream fout("model.out");
        PrintGraph(fout, *graph);
        fout.close();
    }

    std::ifstream fin("pattern.in");
    const auto pattern = ReadPattern(fin, IS_DIRECTED);

    auto vFlags = GetReasonableVertices(*graph, pattern);

//    std::unique_ptr<TSearchProcessorBase> sp(new TBFSearchProcessor(graph, false));
    std::unique_ptr<TSearchProcessorBase> sp(new TCycleDiPathSearchProcessor(graph, false));
    sp->DoSearch(pattern, vFlags);

    {
        std::ofstream fout("match.out");
        fout << sp->GetResults().size() << std::endl;
        for (const auto& match : sp->GetResults()) {
            OutputForPlot(fout, match);
        }
        fout.close();
    }

    return 0;
}
