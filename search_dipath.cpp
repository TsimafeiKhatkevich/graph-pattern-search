#include "search_dipath.h"

void TPathDichotomyChecker::SetupTables(const size_t maxK) {
    if (maxK < 2) {
        return;
    }
    CalcStatus.resize(maxK * KStep, false);
    PathValues.resize(maxK * KStep, false);
}

void TPathDichotomyChecker::InitFlags(const std::vector<char>* vFlags) {
    VFlags = vFlags;
}

bool TPathDichotomyChecker::IsPath(size_t k, size_t x, size_t y) {
    const size_t hash = Hash(k, x, y);
    if (k == 1ul || CalcStatus[hash]) {
        return PathValues[hash];
    }

    const size_t rHash = Hash(k, y, x);
    for (size_t i = 2ul; i < k; i += 2ul) {
        if (IsPath(k-i, x, y)) {
            CalcStatus[hash] = true;
            if (!IsDirected) {
                CalcStatus[rHash] = true;
            }
            return false;
        }
    }
    bool result = false;
    const auto& flagsRef = *VFlags;
    for (size_t z = 0; z < flagsRef.size(); ++z) {
        if (flagsRef[z] != FLAG_NOT_REASONABLE && z != x && z != y) {
            bool exist = (k & 1ul) ? IsPath((k - 1ul) >> 1, x, z) && IsPath((k + 1ul) >> 1, z, y)
                                   : IsPath(k >> 1, x, z) && IsPath(k >> 1, z, y);
            if (exist) {
                result = true;
                SplitVertices[hash] = z;
                if (!IsDirected && !(k & 1)) {
                    SplitVertices[rHash] = z;
                }
                if (flagsRef[z] == FLAG_AVAILABLE) {
                    break;
                } else {
                    continue;
                }
            }
        }
    }
    CalcStatus[hash] = true;
    PathValues[hash] = result;
    if (!IsDirected && !(k & 1)) {
        CalcStatus[rHash] = true;
        PathValues[rHash] = result;
    }
    return result;
}

void TPathDichotomyChecker::RestorePath(size_t k, size_t x, size_t y, std::vector<ui32>& matches) const {
    const size_t hash = Hash(k, x, y);
    if (k == 1ul || PathValues[hash] == false || SplitVertices.count(hash) == 0) {
        return;
    }
    const size_t z = SplitVertices.at(hash);
    if (k & 1ul) {
        RestorePath((k - 1ul) >> 1, x, z, matches);
        matches.push_back(ui32(z));
        RestorePath((k + 1ul) >> 1, z, y, matches);
    } else {
        RestorePath(k >> 1, x, z, matches);
        matches.push_back(ui32(z));
        RestorePath(k >> 1, z, y, matches);
    }
}

bool TPathDichotomyChecker::TryGetCentral(size_t k, size_t x, size_t y, ui32& result) const {
    const size_t hash = Hash(k, x, y);
    if (k == 1ul || PathValues[hash] == false || SplitVertices.count(hash) == 0) {
        return false;
    }
    result = SplitVertices.at(hash);
    return true;
}


bool TKeyPoints::Init() {
    static const size_t n = Points.size();
    for (size_t i = 1; i <= n; ++i) {
        auto& k = Points[n - i];
        while (k < Max && Flags[k] != FLAG_AVAILABLE) {
            ++k;
        }
        if (k < Max) {
            Flags[k] = FLAG_IN_PATTERN;
        } else {
            return false;
        }
    }
    Inited = true;
    return true;
}

bool TKeyPoints::IsValid() const {
    return Inited && Points.back() < Max;
}

bool TKeyPoints::Next() {
    if (!IsValid()) {
        return false;
    }
    static const size_t n = Points.size();
    size_t i = 0;

    for (; i < n; ++i) {
        auto& k = Points[i];
        Flags[k] = FLAG_AVAILABLE;
        ++k;
        while (k < Max && Flags[k] != FLAG_AVAILABLE) {
            ++k;
        }
        if (k < Max) {
            Flags[k] = FLAG_IN_PATTERN;
            break;
        } else {
            k = 0;
        }
    }

    if (i == n) {
        Points.back() = Max;
        return false;
    }

    for (size_t j = 0; j < i; ++j) {
        auto& k = Points[i - j - 1];
        while (k < Max && Flags[k] != FLAG_AVAILABLE) {
            ++k;
        }
        if (k < Max) {
            Flags[k] = FLAG_IN_PATTERN;
        } else {
            Inited = false;
            return false;
        }
    }
    return true;

}

const std::vector<ui32>& TKeyPoints::Tuple() const {
    return Points;
}


bool TCycleDiPathSearchProcessor::DoSearch(const TAdjMatrix& pattern, std::vector<char>& vFlags) {
    static const ui32 graphSize = HostGraph->size();
    const ui32 pSize = pattern.size();
    ui32 halfPSize = pSize >> 1;
    SetupTables(halfPSize + 1);
    InitFlags(&vFlags);

    if (pSize & 1ul) {
        TKeyPoints kp(2, vFlags);
        kp.Init();
        auto& y = kp.Tuple()[0];
        auto& x = kp.Tuple()[1];

        while (kp.IsValid()) {
            if (!IsPath(halfPSize, x, y) || !IsPath(halfPSize + 1, y, x)) {
                kp.Next();
                continue;
            }
            bool goodPath = true;
            for (ui32 i = 1; i < halfPSize && goodPath; ++i) {
                for (ui32 z = 0; z < graphSize; ++z) {
                    if (vFlags[z] != FLAG_AVAILABLE) {
                        continue;
                    }
                    if (IsPath(i, y, z) && IsPath(i+1, z, y)) {
                        goodPath = false;
                        break;
                    }
                    if (IsPath(i, x, z) && IsPath(i+1, z, x)) {
                        goodPath = false;
                        break;
                    }
                }
            }
            if (goodPath == false) {
                kp.Next();
                continue;
            } else {
                Results.push_back(TMatch());
                auto& match = Results.back();
                match.push_back(x);
                RestorePath(halfPSize, x, y, match);
                match.push_back(y);
                RestorePath(halfPSize + 1, y, x, match);
                if (StopOnFirst) {
                    break;
                }
                kp.Next();
            }
        }
    } else {
        --halfPSize;
        TKeyPoints kp(3, vFlags);
        kp.Init();
        auto& x = kp.Tuple()[0];
        auto& z = kp.Tuple()[1];
        auto& v = kp.Tuple()[2];
        ui32 y = ui32(-1);

        while (kp.IsValid()) {
            if (!IsPath(2ul, x, z)) {
                kp.Next();
                continue;
            }
            if (!IsPath(halfPSize, v, x) || !IsPath(halfPSize, z, v)) {
                kp.Next();
                continue;
            }

            if (TryGetCentral(2ul, x, z, y) && vFlags[y] == FLAG_AVAILABLE) {
                vFlags[y] = FLAG_IN_PATTERN;
            } else {
                kp.Next();
                continue;
            }
            bool goodPath = !IsPath(halfPSize, y, v);
            for (ui32 i = 1; i < halfPSize && goodPath; ++i) {
                if (IsPath(i, y, v)) {
                    goodPath = false;
                    break;
                }
                for (ui32 t = 0; t < graphSize; ++t) {
                    if (vFlags[t] != FLAG_AVAILABLE && t != v) {
                        continue;
                    }
                    if (i == 1 && t == y) {
                        continue;
                    }
                    if (IsPath(i, z, t) && IsPath(i, t, x)) {
                        goodPath = false;
                        break;
                    }
                }
            }
            if (goodPath == false) {
                vFlags[y] = FLAG_AVAILABLE;
                kp.Next();
                continue;
            } else {
                Results.push_back({x, y, z});
                auto& match = Results.back();
                RestorePath(halfPSize, z, v, match);
                match.push_back(v);
                RestorePath(halfPSize, v, x, match);
                if (match.size() != pSize) {
                    Results.pop_back();
                    continue;
                }
                if (StopOnFirst) {
                    break;
                }
            } // if goodPath
        } // while kp
    }
    return !Results.empty();
}
