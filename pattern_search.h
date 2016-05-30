#pragma once

#include "search_dipath.h"
#include "search_bf.h"
#include <unordered_map>
#include <unordered_set>

std::vector<char> GetReasonableVertices(const TAdjMatrix& hostGraph, const TAdjMatrix& pattern);


using TCanonicals = std::unordered_set<std::string>;
struct TCanonicalSets {
    TCanonicals All;
    TCanonicals Induced;
};

TCanonicalSets CanonizeMatchedCycles(
        const TAdjMatrix& hostGraph,
        const TAdjMatrix& pattern,
        const std::vector<TSearchProcessorBase::TMatch>& matches);
