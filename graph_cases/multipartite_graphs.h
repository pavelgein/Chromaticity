#pragma once

#include <iterator>
#include <vector>

#include "local_types.h"
#include "math_utils.h"

namespace NMultipartiteGraphs {
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

class TCompleteGraph {
public:
    TCompleteGraph(const std::initializer_list<INT>& components);

    template<class TInputIterator>
    TCompleteGraph(TInputIterator begin, TInputIterator end);

    INT I3Invariant() const;
    INT I4Invariant() const;

    bool operator==(const TCompleteGraph& other) const;

    inline INT ComponentSize(size_t component) const;
    inline size_t ComponentsNumber() const;

private:
    const std::vector<INT> Components;
    mutable INT I3Invariant_;
    mutable INT I4Invariant_;
    INT Edges_;

    INT ComputeI4() const;
};
};

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TEdge& edge);
std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TVertex& node);

namespace std {
    template<>
    struct std::hash<NMultipartiteGraphs::TVertex> {
        std::size_t operator()(const NMultipartiteGraphs::TVertex& node) const {
            return static_cast<std::size_t>(node.ComponentId << 4) ^ (node.VertexId);
        }
    };


    template<>
    struct hash<NMultipartiteGraphs::TEdge> {
        std::size_t operator()(const NMultipartiteGraphs::TEdge& edge) const {
            return std::hash<NMultipartiteGraphs::TVertex>()(edge.First) ^
                   std::hash<NMultipartiteGraphs::TVertex>()(edge.Second);
        }
    };
}
