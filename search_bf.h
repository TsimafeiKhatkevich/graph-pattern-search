#pragma once

#include "common.h"

class TBFSearchProcessor : public TSearchProcessorBase {
private:
    const TEdgeListIndexed Graph;
    const TAdjMatrixFast GraphFast;

public:
    TBFSearchProcessor(std::shared_ptr<TAdjMatrix> graph, bool stopOnFirst = false, bool saveOnlyInduced = false)
        : TSearchProcessorBase(graph, stopOnFirst, saveOnlyInduced),
        Graph(FromTAdjMatrix<TEdgeListIndexed>(*graph)),
        GraphFast(*graph)
    {
    }

    bool DoSearch(const TAdjMatrix& pattern, std::vector<char>& vFlags) override;
};
