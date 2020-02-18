#include "multipartite_graphs.h"

#include "math_utils.h"

#include <sstream>
namespace {
bool IsAdjanced(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    return (first.First == second.First) ||
        (first.First == second.Second) ||
        (first.Second == second.First) ||
        (first.Second == second.Second);
}


bool InSameComponent(const NMultipartiteGraphs::TVertex& first, const NMultipartiteGraphs::TVertex& second) {
    return first.ComponentId == second.ComponentId;
}

NMultipartiteGraphs::TEdge build_up_to_triangle(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    NMultipartiteGraphs::TVertex target1, target2;
    if (InSameComponent(first.First, second.First)) {
        target1 = first.Second;
        target2 = second.Second;
    }

    if (InSameComponent(first.First, second.Second)) {
        target1 = first.Second;
        target2 = second.First;
    }

    if (InSameComponent(first.Second, second.First)) {
        target1 = first.First;
        target2 = second.Second;
    }

    if (InSameComponent(first.Second, second.Second)) {
        target1 = first.First;
        target2 = second.First;
    }
    return NMultipartiteGraphs::TEdge{target1, target2};
}


bool ExactlyOneCommonComponent(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    int commonComponents = 0;
    if (InSameComponent(first.First, second.First))
        ++commonComponents;
    if (InSameComponent(first.First, second.Second))
        ++commonComponents;
    if (InSameComponent(first.Second, second.First))
        ++commonComponents;
    if (InSameComponent(first.Second, second.Second))
        ++commonComponents;
    return (commonComponents == 1);
}


bool IsXi2Subgraph(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    return IsAdjanced(first, second) &&
           ExactlyOneCommonComponent(first, second);
}
}


namespace NMultipartiteGraphs {
INT TCompleteGraph::VerticesCount() const {
    return SumRange(begin(), end());
}


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
    : Components{components}
    , I3Invariant_(0)
    , I4Invariant_(0)
    , Edges_(Sigma(2, Components))
    {
    }

template<class TInputIterator>
TCompleteGraph::TCompleteGraph(TInputIterator begin, TInputIterator end)
    : Components(begin, end)
    , I3Invariant_(0)
    , I4Invariant_(0)
    , Edges_(Sigma(2, Components))
    {
    }

INT TCompleteGraph::I2Invariant() const {
    return Edges_;
}

INT TCompleteGraph::I3Invariant() const {
    if (Components.size() < 3) {
        return 0;
    }

    if (I3Invariant_ == 0) {
        I3Invariant_ = Sigma(3, Components);
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
}

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

std::vector<TEdge> TCompleteGraph::GenerateEdgesBetweenComponents(size_t first, size_t second) const {
    auto firstSize = Components[first];
    auto secondSize = Components[second];
    std::vector<TEdge> edges;
    edges.reserve(firstSize * secondSize);
    for (INT i = 0; i < firstSize; ++i) {
        TVertex firstVertex{first, i};
        for (INT j = 0; j < secondSize; ++j) {
            TVertex secondVertex{second, j};
            edges.emplace_back(firstVertex, secondVertex);
        }
    }

    return edges;
}

std::vector<INT>::const_iterator TCompleteGraph::begin() const {
    return Components.begin();
}

std::vector<INT>::const_iterator TCompleteGraph::end() const {
    return Components.end();
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
    return ((First == other.First) && (Second == other.Second)) || ((First == other.Second) && (Second == other.First));
}


TDenseGraph::TDenseGraph(TCompleteGraph graph, TEdgeSet edgeSet)
    : Graph(std::move(graph))
    , EdgeSet(std::move(edgeSet))
    {
    }



INT TDenseGraph::VerticesCount() const {
    return SumRange(Graph.begin(), Graph.end());
}

INT TDenseGraph::I2Invariant() const {
    return Graph.I2Invariant() - EdgeSet.size();
}

INT TDenseGraph::I3Invariant() const {
    if (Graph.ComponentsNumber() < 3) {
        return 0;
    }

    if (I3Invariant_ == 0) {
        I3Invariant_ = Graph.I3Invariant() - ComputeXi1() + ComputeXi2AndXi3();
    }

    return I3Invariant_;

}

INT TDenseGraph::ComputeXi1() const {
   INT ans = 0;
   INT totalVertices = Graph.VerticesCount();
   for (const auto& edge: EdgeSet) {
        auto firstComponent = edge.First.ComponentId;
        auto secondComponent = edge.Second.ComponentId;
        ans += totalVertices - Graph.ComponentSize(firstComponent) - Graph.ComponentSize(secondComponent);
   }
   return ans;
}


std::vector<INT>::const_iterator TDenseGraph::begin() const {
    return Graph.begin();
}

std::vector<INT>::const_iterator TDenseGraph::end() const {
    return Graph.end();
}

INT TDenseGraph::ComputeXi2AndXi3() const {
    INT xi_3 = 0;
    INT xi_2 = 0;

    for (auto iter1 = EdgeSet.begin(); iter1 !=EdgeSet.end(); ++iter1) {
        for (auto iter2 = iter1; iter2 != EdgeSet.end(); ++iter2) {
            if (IsXi2Subgraph(*iter1, *iter2)) {
                TEdge additional_edge = build_up_to_triangle(*iter1, *iter2);
                if (EdgeSet.find(additional_edge) != EdgeSet.end()) {
                    ++xi_3;
                } // TO DO: clean up
                ++xi_2;
            }
        }
    }

    xi_2 = xi_2 - xi_3;
    xi_3 /= 3;

    return 2 * xi_3 + xi_2;
}

INT TDenseGraph::I4Invariant() const {
    return ComputeI4TwoParts() + ComputeI4ThreeParts();
}

INT TDenseGraph::ComputeI4TwoParts() const {
    INT answer = 0;
    for (const auto [firstComponent, secondComponent] : TPairGenerator(Graph.ComponentsNumber())) {
        assert(firstComponent < secondComponent);
        const auto firstComponentSize = Graph.ComponentSize(firstComponent);
        const auto secondComponentSize = Graph.ComponentSize(secondComponent);
        for (const auto [firstCompFirstVertex, firstCompSecondVertex] : TPairGenerator(firstComponentSize)) {
           auto v11 = TVertex(firstComponent, firstCompFirstVertex);
           auto v12 = TVertex(firstComponent, firstCompSecondVertex);
           for (const auto [secondComponentFirstVertex, secondComponentSecondVertex] : TPairGenerator(secondComponentSize)) {
               auto v21 = TVertex(secondComponent, secondComponentFirstVertex);
               auto v22 = TVertex(secondComponent, secondComponentSecondVertex);

               TEdge edge1 = TEdge(v11, v21);
               TEdge edge2 = TEdge(v11, v22);
               TEdge edge3 = TEdge(v12, v21);
               TEdge edge4 = TEdge(v12, v22);

               if (!IsEdgeDeleted(edge1) && !IsEdgeDeleted(edge2) && !IsEdgeDeleted(edge3) && !IsEdgeDeleted(edge4)) {
                   ++answer;
               }
           }
        }
    }

    return answer;
}

INT TDenseGraph::ComputeI4ThreeParts() const {
    INT answer = 0;
    for (const auto [leftComponent, rightComponent] : TPairGenerator(Graph.ComponentsNumber())) {
        for (size_t leftIndex = 0; leftIndex != Graph.ComponentSize(leftComponent); ++leftIndex) {
            auto leftVertex = TVertex(leftComponent, leftIndex);
            for (size_t rightIndex = 0; rightIndex != Graph.ComponentSize(rightComponent); ++rightIndex) {
                auto rightVertex = TVertex(rightComponent, rightIndex);
                if (!IsEdgeDeleted(TEdge(leftVertex, rightVertex))) {
                    continue;
                }

                for (size_t middleComponent = 0; middleComponent != Graph.ComponentsNumber(); ++middleComponent) {
                   if ((middleComponent == leftComponent) || (middleComponent == rightComponent)) {
                       continue;
                   }

                   for (const auto [firstMiddle, secondMiddle] : TPairGenerator(Graph.ComponentSize(middleComponent))) {
                       auto firstMiddleVertex = TVertex(middleComponent, firstMiddle);
                       auto secondMiddleVertex = TVertex(middleComponent, secondMiddle);
                       auto edge1 = TEdge(leftVertex, firstMiddleVertex);
                       auto edge2 = TEdge(leftVertex, secondMiddleVertex);
                       auto edge3 = TEdge(rightVertex, firstMiddleVertex);
                       auto edge4 = TEdge(rightVertex, secondMiddleVertex);

                       if (!IsEdgeDeleted(edge1) && !IsEdgeDeleted(edge2) && !IsEdgeDeleted(edge3) && !IsEdgeDeleted(edge4)) {
                           ++answer;
                       }
                   }
                }
            }
        }
    }

    return answer;
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

std::ostream& operator<<(std::ostream& outp, const NMultipartiteGraphs::TCompleteGraph& graph) {
    outp << "Graph(";
    for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
        if (iter != graph.begin()) {
            outp << ",";
        }
        outp << *iter;
    }
    outp << ")";
    return outp;
}
