#pragma once

#include "common.h"

class TBFSearchProcessor : public TSearchProcessorBase {
private:
    const TEdgeListIndexed Graph;
    const TAdjMatrixFast GraphFast;

public:
    TBFSearchProcessor(std::shared_ptr<TAdjMatrix> graph, bool searchForInduced = false)
        : TSearchProcessorBase(searchForInduced, graph),
        Graph(FromTAdjMatrix<TEdgeListIndexed>(*graph)),
        GraphFast(*graph)
    {
    }

    TResult Find(const TAdjMatrix& pattern, std::vector<char>& vFlags) override;
};
