#include "multipartite_graphs.h"

#include "math_utils.h"


INT TMultipartiteGraph::ComputeI4() const {
    INT ans = 0;
    for (size_t i = 0; i != Components.size() - 1; ++i) {
        for (size_t j = i + 1; j != Components.size(); ++j) {
            ans += c_n_2(Components[i]) * c_n_2(Components[j]);
        }
    }
    return ans;
}

TMultipartiteGraph::TMultipartiteGraph(const std::initializer_list<INT> components)
    : I3Invariant_(0)
    , I4Invariant_(0)
    , Edges_(sigma2<INT, std::vector<INT>>(Components))
    , Components(components)
    {
    }

INT TMultipartiteGraph::I3Invariant() const {
    if (Components.size() < 3) {
        return 0;
    }

    if (I3Invariant_ == 0) {
        I3Invariant_ = sigma3<INT, std::vector<INT>>(Components);
    }

    return I3Invariant_;
}

INT TMultipartiteGraph::I4Invariant() const {
    if (Components.size() < 2) {
        return 0;
    }

    if (I4Invariant_ == 0) {
        I4Invariant_ = ComputeI4();
    }

    return I4Invariant_;
};
