#pragma once

#include "search_dipath.h"
#include "search_bf.h"
#include <unordered_map>

std::vector<char> GetReasonableVertices(const TAdjMatrix& hostGraph, const TAdjMatrix& pattern);
