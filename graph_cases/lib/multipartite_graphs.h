#pragma once

#include <vector>
#include <unordered_set>

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


class IGraph {
public:
    virtual INT VerticesCount() const=0;
    virtual INT I2Invariant() const=0;
    virtual INT I3Invariant() const=0;
    virtual INT I4Invariant() const=0;
    virtual ~IGraph() = default;
};

class TCompleteGraph: public IGraph {
public:
    TCompleteGraph() = default;

    TCompleteGraph(TCompleteGraph&& other) = default;

    TCompleteGraph(const std::initializer_list<INT>& components);

    template<class TInputIterator>
    TCompleteGraph(TInputIterator begin, TInputIterator end)
        : Components(begin, end)
        , I3Invariant_(0)
        , I4Invariant_(0)
        , Edges_(Sigma(2, Components))
    {
    }


    virtual INT VerticesCount() const override;
    virtual INT I2Invariant() const override;
    virtual INT I3Invariant() const override;
    virtual INT I4Invariant() const override;

    bool operator==(const TCompleteGraph& other) const;

    INT ComponentSize(size_t component) const;
    size_t ComponentsNumber() const;

    std::vector<TEdge> GenerateEdgesBetweenComponents(size_t first, size_t second) const;

    std::vector<TEdge> GenerateAllEdges() const;

    std::vector<INT>::const_iterator begin() const;

    std::vector<INT>::const_iterator end() const;

private:
    const std::vector<INT> Components;
    mutable INT I3Invariant_;
    mutable INT I4Invariant_;
    INT Edges_;

    INT ComputeI4() const;
};
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TEdge& edge);
std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TVertex& node);
std::ostream& operator<<(std::ostream& outp, const NMultipartiteGraphs::TCompleteGraph& graph);

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

namespace NMultipartiteGraphs {

using TEdgeSet = std::unordered_set<TEdge>;

/*
 * Graph which is obtained from complete multipartite graph
 * by deleting some relatively small set of edges
 */
class TDenseGraph : public IGraph {
public:
    TDenseGraph() = default;

    TDenseGraph(TDenseGraph&& other) = default;

    TDenseGraph(const TDenseGraph& other);

    void operator=(TDenseGraph&& other) {
        Graph = other.Graph;
        EdgeSet = std::move(other.EdgeSet);
        other.Graph = nullptr;
    }

    TDenseGraph(const TCompleteGraph& graph, TEdgeSet edgeSet);

    virtual INT VerticesCount() const override;
    virtual INT I2Invariant() const override;
    virtual INT I3Invariant() const override;
    virtual INT I4Invariant() const override;

    INT ComponentSize(size_t component) const;
    size_t ComponentsNumber() const;

    //std::vector<TEdge> GenerateEdgesBetweenComponents(size_t first, size_t second) const;
    std::vector<INT>::const_iterator begin() const;
    std::vector<INT>::const_iterator end() const;

    const TEdgeSet& DeletedEdges() const {
        return EdgeSet;
    }

    bool IsEdgeDeleted(const TEdge& edge) const {
        return EdgeSet.find(edge) != EdgeSet.end();
    }

    ~TDenseGraph() override = default;

private:
    const TCompleteGraph* Graph;
    TEdgeSet EdgeSet;

    mutable INT Vertices_ = 0;
    mutable INT Edges_ = 0;
    mutable INT I2Invariant_ = 0;
    mutable INT I3Invariant_ = 0;
    mutable INT I4Invariant_ = 0;

    INT ComputeXi1() const;
    INT ComputeXi2AndXi3() const;

    INT ComputeI4TwoParts() const;
    INT ComputeI4ThreeParts() const;
};
};

