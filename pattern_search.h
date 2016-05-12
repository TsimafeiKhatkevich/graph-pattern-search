#pragma once

#include "common.h"
#include <unordered_map>

std::vector<char> GetReasonableVertices(const TAdjMatrix& hostGraph, const TAdjMatrix& pattern);
