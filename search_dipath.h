#pragma once

#include "common.h"
#include <unordered_map>

class TPathDichotomyChecker {
private:
    size_t VStep;
    size_t KStep;
    bool IsDirected;
    std::vector<bool> CalcStatus;
    std::vector<bool> PathValues;
    const std::vector<char>* VFlags;
    std::unordered_map<size_t, size_t> SplitVertices;

    inline size_t Hash(size_t k, size_t x, size_t y) const {
        return y + x*VStep + (k-1ul)*KStep;
    }

public:
    TPathDichotomyChecker(const TAdjMatrixFast& graph, bool isDirected = false)
        : VStep(graph.Size()),
        KStep(VStep * VStep),
        IsDirected(isDirected),
        CalcStatus(KStep, true),
        PathValues(graph.GetData()),
        VFlags(nullptr)
    {
    }

    void SetupTables(const size_t maxK);
    void InitFlags(const std::vector<char>* vFlags);

    bool IsPath(size_t k, size_t x, size_t y);

    void RestorePath(size_t k, size_t x, size_t y, std::vector<ui32>& matches) const;

    bool TryGetCentral(size_t k, size_t x, size_t y, ui32& result) const;
};

class TKeyPoints {
private:
    std::vector<ui32> Points;
    std::vector<char>& Flags;
    const ui32 Max;
    bool Inited;

public:
    TKeyPoints(size_t n, std::vector<char>& flags)
        : Points(n),
        Flags(flags),
        Max(flags.size()),
        Inited(false)
    {
    }

    bool Init();
    bool IsValid() const;
    bool Next();
    const std::vector<ui32>& Tuple() const;
};

class TCycleDiPathSearchProcessor : public TSearchProcessorBase, public TPathDichotomyChecker {
public:
    TCycleDiPathSearchProcessor(std::shared_ptr<TAdjMatrix> graph)
        : TSearchProcessorBase(false, graph),
        TPathDichotomyChecker(TAdjMatrixFast(*graph))
    {
    }

    TResult Find(const TAdjMatrix& pattern, std::vector<char>& vFlags) override;
};

