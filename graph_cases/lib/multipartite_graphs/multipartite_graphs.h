#pragma once

#include "local_types.h"
#include "graph.h"
#include "math_utils/sigma.h"

#include <vector>
#include <unordered_set>
#include <memory>


namespace NMultipartiteGraphs {
class TCompleteGraph: public IGraph {
public:
    TCompleteGraph() = default;

    TCompleteGraph(TCompleteGraph&& other) = default;

    TCompleteGraph(const std::initializer_list<INT>& components);

    TCompleteGraph(const TCompleteGraph& other) = default;

    TCompleteGraph& operator=(const TCompleteGraph& other) = default;

    TCompleteGraph(std::vector<INT> components)
        : Components(std::move(components))
        , I3Invariant_(0)
        , I4Invariant_(0)
        , PtInvariant_(0)
        , Edges_(Sigma(2, Components))
    {
    }

    template<class TInputIterator>
    TCompleteGraph(TInputIterator begin, TInputIterator end)
        : Components(begin, end)
        , I3Invariant_(0)
        , I4Invariant_(0)
        , PtInvariant_(0)
        , Edges_(Sigma(2, Components))
    {
    }


    INT VerticesCount() const override;

    INT I2Invariant() const override;

    INT I3Invariant() const override;

    INT I4Invariant() const override;

    INT PtInvariant() const override;

    bool operator==(const TCompleteGraph& other) const;

    INT ComponentSize(size_t component) const;
    size_t ComponentsNumber() const;

    std::vector<TEdge> GenerateEdgesBetweenComponents(size_t first, size_t second) const;

    std::vector<TEdge> GenerateAllEdges() const;

    std::vector<INT>::const_iterator begin() const;

    std::vector<INT>::const_iterator end() const;

private:
    INT CalculatePtInvariant() const;

    std::vector<INT> Components;
    mutable INT I3Invariant_;
    mutable INT I4Invariant_;
    mutable INT PtInvariant_;
    INT Edges_;


    INT ComputeI4() const;
};
}

std::ostream& operator<<(std::ostream& outp, const NMultipartiteGraphs::TCompleteGraph& graph);

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

    INT VerticesCount() const override;

    INT I2Invariant() const override;

    INT I3Invariant() const override;

    INT I4Invariant() const override;

    INT PtInvariant() const override;

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

    void SwapVerticesInplace(size_t componentId, size_t firstVertex, size_t secondVertex);

    TDenseGraph SwapVertices(size_t componentId, size_t firstVertex, size_t secondVertex) const;

    std::pair<TDenseGraph, std::unique_ptr<TCompleteGraph>> ContractEdge(const TEdge& edge) const;

    static TEdgeSet SwapVerticesInSet(const TEdgeSet& edgeSet, size_t componentId, size_t firstVertex, size_t secondVertex);

    ~TDenseGraph() override = default;

private:
    const TCompleteGraph* Graph;
    TEdgeSet EdgeSet;

    mutable INT I3Invariant_ = 0;
    mutable INT I4Invariant_ = 0;
    mutable INT PtInvariant_ = 0;

    INT ComputePtInvariant() const;
    INT CountGarlands() const;
    bool IsInterestingGarland(const std::vector<const TEdge*>& edges) const;

    INT ComputeXi1() const;
    INT ComputeXi2AndXi3() const;

    INT ComputeI4TwoParts() const;
    INT ComputeI4ThreeParts() const;
};
}

