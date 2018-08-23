#include "multipartite_graphs.h"

#include "math_utils.h"

#include <sstream>


namespace NMultipartiteGraphs {
INT TCompleteGraph::ComputeI4() const {
    INT ans = 0;
    for (size_t i = 0; i != Components.size() - 1; ++i) {
        for (size_t j = i + 1; j != Components.size(); ++j) {
            ans += c_n_2(Components[i]) * c_n_2(Components[j]);
        }
    }
    return ans;
}

TCompleteGraph::TCompleteGraph(const std::initializer_list<INT>& components)
    : Components(components)
    , I3Invariant_(0)
    , I4Invariant_(0)
    , Edges_(sigma2(Components))
    {
    }

template<class TInputIterator>
TCompleteGraph::TCompleteGraph(TInputIterator begin, TInputIterator end)
    : Components(begin, end)
    , I3Invariant_(0)
    , I4Invariant_(0)
    , Edges_(sigma2(Components))
    {
    }

INT TCompleteGraph::I3Invariant() const {
    if (Components.size() < 3) {
        return 0;
    }

    if (I3Invariant_ == 0) {
        I3Invariant_ = sigma3<INT, std::vector<INT>>(Components);
    }

    return I3Invariant_;
}

INT TCompleteGraph::I4Invariant() const {
    if (Components.size() < 2) {
        return 0;
    }

    if (I4Invariant_ == 0) {
        I4Invariant_ = ComputeI4();
    }

    return I4Invariant_;
};

bool TCompleteGraph::operator==(const TCompleteGraph& other) const {
    if (Components.size() != other.Components.size()) {
        return false;
    }

    for (size_t i = 0; i < Components.size(); ++i) {
        if (Components[i] != other.Components[i]) {
            return false;
        }
    }

    return true;
}

INT TCompleteGraph::ComponentSize(size_t component) const {
    return Components[component];
}


size_t TCompleteGraph::ComponentsNumber() const {
    return Components.size();
}

TVertex::TVertex()
    : ComponentId(0)
    , VertexId(0)
    {
    }


TVertex::TVertex(size_t componentId, INT vertexId)
    : ComponentId(componentId)
    , VertexId(vertexId)
    {
    }

bool TVertex::operator==(const TVertex& other) const {
    return (ComponentId == other.ComponentId) && (VertexId == other.VertexId);
}


TEdge::TEdge()
    : First()
    , Second()
    {
    }

TEdge::TEdge(TVertex first, TVertex second)
    : First(first)
    , Second(second)
    {
    }

bool TEdge::operator==(const TEdge& other) const {
    return (First == other.First) && (Second == other.Second);
}
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TEdge& edge) {
    std::stringstream ss;
    ss << "TEdge(" << edge.First << ", " << edge.Second << ")";
    os << ss.str();
    return os;
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TVertex& node) {
    std::stringstream ss;
    ss << "TNode(" << node.ComponentId << ", " << node.VertexId << ")";
    os << ss.str();
    return os;
}
