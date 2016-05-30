#include "pattern_search.h"
#include "er_model.h"

#include <map>
#include <fstream>

static const ui32 N_MIN = 10;
static const ui32 N_MAX = 400;
static const double STEP_MULT = 0.5;

static const double P_STEP = 1.0;
static const double PC_MAX = 4.0;

static const ui32 SAMPLES = 100;

static const double MLOG_MULT = 1.5;
static const ui32 M_STEP = 1;

static const bool IS_DIRECTED = false;
static const bool BINARY_STAT = true;

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

void TestPrecision() {
    TAdjMatrix minPattern(3);
    minPattern[0] = {1, 2};
    minPattern[1] = {0, 2};
    minPattern[2] = {0, 1};
    std::ofstream fout("stats.out");
    fout << "n p m bf_all bf_induced dp_all dp_induced intersect_all intersect_ind " << std::endl;

    for (ui32 n = N_MIN; n <= N_MAX; n += ui32(n * STEP_MULT + 1)) {
        for (double p = 1.0; p <= PC_MAX; p += P_STEP) {
            const ui32 maxM = std::min(ui32(::log2(double(n))*MLOG_MULT + 1.0), n >> 1);
            std::vector<std::vector<double> > curStats(maxM - 3, std::vector<double>(6, 0.0));

            for (ui32 iSample = 0; iSample < SAMPLES; ++iSample) {
                std::cout << "n = " << n << "\tp*n = " << p << "\tsample " << iSample << std::endl;
                TERModelGenerator gGenerator;
                const auto graph = gGenerator(n, p / n);
                std::unique_ptr<TSearchProcessorBase> bf(new TBFSearchProcessor(graph, BINARY_STAT));
                std::unique_ptr<TSearchProcessorBase> diPath(new TCycleDiPathSearchProcessor(graph, BINARY_STAT));
                TAdjMatrix pattern = minPattern;

                for (ui32 m = 3; m < maxM; m += M_STEP) {
                    auto vFlags = GetReasonableVertices(*graph, pattern);
                    std::vector<char> flagsCopy = vFlags;

                    bf->ClearResults();
                    const bool isCycle = bf->DoSearch(pattern, vFlags);
                    const auto& bfResult = bf->GetResults();
                    const auto bfCanonized = CanonizeMatchedCycles(*graph, pattern, bfResult);
                    curStats[m - 3][0] += BINARY_STAT ? static_cast<double>(isCycle)
                                                      : bfCanonized.All.size();
                    curStats[m - 3][1] += BINARY_STAT ? static_cast<double>(!bfCanonized.Induced.empty())
                                                      : bfCanonized.Induced.size();

                    diPath->ClearResults();
                    const bool isFormula = diPath->DoSearch(pattern, flagsCopy);
                    const auto& diPResult = diPath->GetResults();
                    const auto diPCanonized = CanonizeMatchedCycles(*graph, pattern, diPResult);
                    curStats[m - 3][2] += BINARY_STAT ? static_cast<double>(isFormula)
                                                      : diPCanonized.All.size();
                    curStats[m - 3][3] += BINARY_STAT ? static_cast<double>(isFormula)
                                                      : diPCanonized.Induced.size();

                    size_t intAll = 0;
                    size_t intInduced = 0;
                    for (const auto& el : diPCanonized.All) {
                        if (bfCanonized.All.count(el) > 0) {
                            ++intAll;
                        }
                    }
                    for (const auto& el : diPCanonized.Induced) {
                        if (bfCanonized.Induced.count(el) > 0) {
                            ++intInduced;
                        }
                    }
                    curStats[m - 3][4] += BINARY_STAT ? static_cast<double>(isFormula && isCycle) : intAll;
                    curStats[m - 3][5] += BINARY_STAT ? static_cast<double>(isFormula && !bfCanonized.Induced.empty())
                                                      : intInduced;

                    pattern.back() = {m - 2, m};
                    pattern.push_back({m - 1, 0});
                    pattern[0] = {1, m};
                } // for cycle size
            } // for sample
            for (size_t i = 0; i < curStats.size(); ++i) {
                fout << n << " " << p << " " << i + 3 << " ";
                for (auto& stat : curStats[i]) {
                    stat /= SAMPLES;
                    fout << stat << " ";
                }
                fout << std::endl;
            }
            curStats.clear();
        } // for p
    } // for n
}

int main() {
    TestPrecision();
    return 0;

    TERModelGenerator gGenerator;
    const auto graph = gGenerator(15, 4.0/15.0);
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
    const auto& result = sp->GetResults();
    const auto canonized = CanonizeMatchedCycles(*graph, pattern, result);
    {
        std::ofstream fout("match.out");
        fout << canonized.All.size() << std::endl;
        for (const auto& match : canonized.All) {
            fout << match << std::endl;
        }
        fout.close();
    }

    return 0;
}
