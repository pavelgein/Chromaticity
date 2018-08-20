#pragma once

#include <vector>

#include "local_types.h"
#include "math_utils.h"

class TMultipartiteGraph {
private:
    mutable INT I3Invariant_;
    mutable INT I4Invariant_;
    INT Edges_;

    INT ComputeI4() const;

public:
    const std::vector<INT> Components;
    TMultipartiteGraph(const std::initializer_list<INT> components);

    INT I3Invariant() const;
    INT I4Invariant() const;
};

