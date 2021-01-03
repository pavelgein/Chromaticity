#include "graph.h"

namespace NMultipartiteGraphs {
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

    bool TVertex::operator!=(const TVertex& other) const {
        return !(*this == other);
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
        return ((First == other.First) && (Second == other.Second)) || ((First == other.Second) && (Second == other.First));
    }
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TEdge& edge) {
    os << "TEdge(" << edge.First << ", " << edge.Second << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TVertex& node) {
    os << "TNode(" << node.ComponentId << ", " << node.VertexId << ")";
    return os;
}