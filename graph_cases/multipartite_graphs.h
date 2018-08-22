#pragma once

#include <iterator>
#include <vector>

#include "local_types.h"
#include "math_utils.h"

class TMultipartiteGraph {
public:
    struct TVertex {
        size_t ComponentId;
        INT VertexId;
        TVertex(size_t ComponentId, INT VertexId);
        TVertex();
        bool operator==(const TVertex& other) const;
    };

    struct TEdge {
        TVertex First;
        TVertex Second;
        TEdge();
        TEdge(TVertex first, TVertex second);
        bool operator==(const TEdge& other) const;
    };

public:
    TMultipartiteGraph(const std::initializer_list<INT>& components);

    template<class TInputIterator>
    TMultipartiteGraph(TInputIterator begin, TInputIterator end);

    INT I3Invariant() const;
    INT I4Invariant() const;

    bool operator==(const TMultipartiteGraph& other) const;

    inline INT ComponentSize(size_t component) const;
    inline size_t ComponentsNumber() const;

private:
    const std::vector<INT> Components;
    mutable INT I3Invariant_;
    mutable INT I4Invariant_;
    INT Edges_;

    INT ComputeI4() const;
};

std::ostream& operator<<(std::ostream& os, const TMultipartiteGraph::TEdge& edge);
std::ostream& operator<<(std::ostream& os, const TMultipartiteGraph::TVertex& node);

namespace std {
    template<>
    struct std::hash<TMultipartiteGraph::TVertex> {
        std::size_t operator()(const TMultipartiteGraph::TVertex& node) const {
            return static_cast<std::size_t>(node.ComponentId << 4) ^ (node.VertexId);
        }
    };


    template<>
    struct hash<TMultipartiteGraph::TEdge> {
        std::size_t operator()(const TMultipartiteGraph::TEdge& edge) const {
            return std::hash<TMultipartiteGraph::TVertex>()(edge.First) ^
                   std::hash<TMultipartiteGraph::TVertex>()(edge.Second);
        }
    };
}

