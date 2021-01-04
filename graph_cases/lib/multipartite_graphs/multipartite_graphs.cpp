#include "multipartite_graphs.h"
#include "acyclic_orintations.h"

#include <autoindexer/autoindexer.h>
#include <binomial_coefficients/binomial_coefficients.h>
#include <singleton/singleton.h>

#include "math_utils/subsets.h"
#include "math_utils/combinatorics.h"
#include "math_utils/sigma.h"
#include "math_utils/sum.h"

#include <ostream>
#include <unordered_map>
#include <queue>


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
            ans += BinomialCoefficient(Components[i], 2) * BinomialCoefficient(Components[j], 2);
        }
    }
    return ans;
}

TCompleteGraph::TCompleteGraph(const std::initializer_list<INT>& components)
    : Components{components}
    , I3Invariant_(0)
    , I4Invariant_(0)
    , PtInvariant_(0)
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


INT TCompleteGraph::PtInvariant() const {
    if (PtInvariant_ == 0) {
        PtInvariant_ = CalculatePtInvariant();
    }

    return PtInvariant_;
}

INT TCompleteGraph::CalculatePtInvariant() const {
    INT result = 0;
    for (auto component : Components) {
        result += 1ull << (component - 1);
    }

    return result - ComponentsNumber();
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

std::vector<TEdge> TCompleteGraph::GenerateAllEdges() const {
    std::vector<TEdge> edges;
    for (size_t i = 0; i + 1 != ComponentsNumber(); ++i) {
        for (size_t j = i + 1; j != ComponentsNumber(); ++j) {
            auto currentEdges = GenerateEdgesBetweenComponents(i, j);
            edges.insert(edges.end(), currentEdges.begin(), currentEdges.end());
        }
    }

    return edges;
}

INT TCompleteGraph::CountAcyclicOrientations() const {
    return TSingleton<TCompleteGraphAcyclicOrientationsCounter>::Instance()(Components);
}


TDenseGraph::TDenseGraph(const TCompleteGraph& graph, TEdgeSet edgeSet)
    : Graph(&graph)
    , EdgeSet(std::move(edgeSet))
{
}

INT TDenseGraph::VerticesCount() const {
    return Graph->VerticesCount();
}

INT TDenseGraph::I2Invariant() const {
    return Graph->I2Invariant() - EdgeSet.size();
}

INT TDenseGraph::I3Invariant() const {
    if (Graph->ComponentsNumber() < 3) {
        return 0;
    }

    if (I3Invariant_ == 0) {
        I3Invariant_ = Graph->I3Invariant() - ComputeXi1() + ComputeXi2AndXi3();
    }

    return I3Invariant_;

}

INT TDenseGraph::ComputeXi1() const {
   INT ans = 0;
   INT totalVertices = Graph->VerticesCount();
   for (const auto& edge: EdgeSet) {
        auto firstComponent = edge.First.ComponentId;
        auto secondComponent = edge.Second.ComponentId;
        ans += totalVertices - Graph->ComponentSize(firstComponent) - Graph->ComponentSize(secondComponent);
   }
   return ans;
}


std::vector<INT>::const_iterator TDenseGraph::begin() const {
    return Graph->begin();
}

std::vector<INT>::const_iterator TDenseGraph::end() const {
    return Graph->end();
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
    if (I4Invariant_ == 0) {
        I4Invariant_ = ComputeI4TwoParts() + ComputeI4ThreeParts();
    }
    return I4Invariant_;
}

INT TDenseGraph::ComputeI4TwoParts() const {
    INT answer = 0;
    for (const auto [firstComponent, secondComponent] : TPairGenerator(Graph->ComponentsNumber())) {
        const auto firstComponentSize = Graph->ComponentSize(firstComponent);
        const auto secondComponentSize = Graph->ComponentSize(secondComponent);
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
    for (const auto [leftComponent, rightComponent] : TPairGenerator(Graph->ComponentsNumber())) {
        for (size_t leftIndex = 0; leftIndex != Graph->ComponentSize(leftComponent); ++leftIndex) {
            auto leftVertex = TVertex(leftComponent, leftIndex);
            for (size_t rightIndex = 0; rightIndex != Graph->ComponentSize(rightComponent); ++rightIndex) {
                auto rightVertex = TVertex(rightComponent, rightIndex);
                if (!IsEdgeDeleted(TEdge(leftVertex, rightVertex))) {
                    continue;
                }

                for (size_t middleComponent = 0; middleComponent != Graph->ComponentsNumber(); ++middleComponent) {
                   if ((middleComponent == leftComponent) || (middleComponent == rightComponent)) {
                       continue;
                   }

                   for (const auto [firstMiddle, secondMiddle] : TPairGenerator(Graph->ComponentSize(middleComponent))) {
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

INT TDenseGraph::PtInvariant() const {
    if (PtInvariant_ == 0) {
        PtInvariant_ = ComputePtInvariant();
    }

    return PtInvariant_;
}

INT TDenseGraph::ComputePtInvariant() const {
    return Graph->PtInvariant() + CountGarlands();
}

INT TDenseGraph::CountGarlands() const {
    INT result = 0;
    for (const auto& subset : TSubsetGenerator(EdgeSet.begin(), EdgeSet.end())) {
        if (IsInterestingGarland(subset)) {
            result += 1;
        }
    }
    return result;
}

bool TDenseGraph::IsInterestingGarland(const std::vector<const TEdge*>& edges) const {
    if (edges.size() == 0) {
        return false;
    }

    TAutoIndexer<TVertex> vertexToInd;
    std::unordered_set<TEdge> edgeSet;
    for (const TEdge* edge : edges) {
        vertexToInd.GetIndex(edge->First);
        vertexToInd.GetIndex(edge->Second);
        edgeSet.insert(*edge);
    }

    size_t n = vertexToInd.Size();

    size_t numberOfDestoryed = 0;
    {
        std::unordered_map<size_t, size_t> deletedInComponents;
        for (size_t i = 0; i != n; ++i) {
            deletedInComponents[vertexToInd.GetKey(i).ComponentId]++;
        }

        for (const auto [componentId, deleted] : deletedInComponents) {
            if (deleted == Graph->ComponentSize(componentId)) {
                numberOfDestoryed += 1;
            }
        }
    }

    std::vector<size_t> colors(n, 0);
    std::vector<std::vector<const TVertex*>> graph(n);
    for (const TEdge* edge : edges) {
        auto firstIndex = vertexToInd.GetIndex(edge->First);
        auto secondIndex = vertexToInd.GetIndex(edge->Second);
        graph[firstIndex].push_back(&edge->Second);
        graph[secondIndex].push_back(&edge->First);
    }

    size_t currentColor = 0;
    for (size_t i = 0; i != n; ++i) {
        if (colors[i] != 0) {
            continue;
        }

        ++currentColor;

        std::unordered_map<size_t, std::vector<const TVertex*>> vertexByComponents;
        std::queue<size_t> q;

        {
            const TVertex& currentVertex = vertexToInd.GetKey(i);
            vertexByComponents[currentVertex.ComponentId].push_back(&currentVertex);
            colors[i] = currentColor;
        }

        q.push(i);
        while (!q.empty()) {
            auto current = q.front();
            q.pop();
            for (const TVertex* vertex : graph[current]) {
                auto index = vertexToInd.GetIndex(*vertex);
                if (colors[index] == 0) {
                    q.push(index);
                    colors[index] = currentColor;
                    vertexByComponents[vertex->ComponentId].push_back(vertex);
                }
            }
        }

        for (auto firstComponentIter = vertexByComponents.begin(); std::next(firstComponentIter) != vertexByComponents.end(); ++firstComponentIter) {
            const auto& firstComponent = firstComponentIter->second;
            for (auto secondComponentIter = std::next(firstComponentIter); secondComponentIter != vertexByComponents.end(); ++secondComponentIter) {
                const auto& secondComponent = secondComponentIter->second;
                for (const TVertex* firstVertex : firstComponent) {
                    for (const TVertex* secondVertex: secondComponent) {
                        if (edgeSet.find({*firstVertex, *secondVertex}) == edgeSet.end()) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return numberOfDestoryed + 1 == currentColor;
}

TDenseGraph::TDenseGraph(const TDenseGraph& other)
    : Graph(other.Graph)
    , EdgeSet(other.EdgeSet)
{
}

TDenseGraph TDenseGraph::SwapVertices(size_t componentId, size_t firstVertex, size_t secondVertex) const {
    TDenseGraph other(*this);
    other.SwapVerticesInplace(componentId, firstVertex, secondVertex);
    return other;
}

void TDenseGraph::SwapVerticesInplace(size_t componentId, size_t firstVertex, size_t secondVertex) {
    EdgeSet = SwapVerticesInSet(EdgeSet, componentId, firstVertex, secondVertex);
}

std::pair<TDenseGraph, std::unique_ptr<TCompleteGraph>> TDenseGraph::ContractEdge(const TEdge& edge) const {
    std::vector<INT> newComponents{Graph->begin(), Graph->end()};
    newComponents.push_back(1);

    TEdgeSet newEdgeSet = EdgeSet;
    auto swapWithLast = [&newEdgeSet, this](const TVertex& vertex) mutable {
        auto component = vertex.ComponentId;
        auto lastInComponent = ComponentSize(component) - 1;
        newEdgeSet = SwapVerticesInSet(newEdgeSet, component, vertex.VertexId, lastInComponent);

        return TVertex(component, lastInComponent);
    };

    TVertex first = swapWithLast(edge.First);
    TVertex second = swapWithLast(edge.Second);
    TVertex newVertex(newComponents.size() - 1, 0);

    TEdge oldEdge(first, second);

    TCompleteGraph tempGraph(newComponents);
    TDenseGraph tempDenseGraph(tempGraph, newEdgeSet);

    TEdgeSet finalEdgeSet;
    for (const auto& edge : newEdgeSet) {
        if (oldEdge == edge) {
            continue;
        }

        if (edge.First == first) {
            TEdge otherEdge(second, edge.Second);
            if (tempDenseGraph.IsEdgeDeleted(otherEdge)) {
                finalEdgeSet.emplace(newVertex, edge.Second);
            }
        } else if (edge.First == second) {
            TEdge otherEdge(first, edge.Second);
            if (tempDenseGraph.IsEdgeDeleted(otherEdge)) {
                finalEdgeSet.emplace(newVertex, edge.Second);
            }
        } else if (edge.Second == first) {
            TEdge otherEdge(second, edge.First);
            if (tempDenseGraph.IsEdgeDeleted(otherEdge)) {
                finalEdgeSet.emplace(edge.First, newVertex);
            }
        } else if (edge.Second == second) {
            TEdge otherEdge(first, edge.First);
            if (tempDenseGraph.IsEdgeDeleted(otherEdge)) {
                finalEdgeSet.emplace(edge.First, newVertex);
            }
        } else {
            finalEdgeSet.emplace(edge);
        }
    }

    auto removeLastVertex = [&finalEdgeSet, &newComponents](size_t componentId) mutable {
       --newComponents[componentId];
       if (newComponents[componentId] == 0) {
            TEdgeSet newEdgeSet;
            for (const auto& edge : finalEdgeSet) {
                if (edge.First.ComponentId == componentId) {
                    newEdgeSet.emplace(TVertex(edge.First.ComponentId - 1, edge.First.VertexId), edge.Second);
                } else if (edge.Second.ComponentId == componentId) {
                    newEdgeSet.emplace(edge.First, TVertex(edge.Second.ComponentId - 1, edge.Second.VertexId));
                } else {
                    newEdgeSet.emplace(edge);
                }
            }
       }
    };

    removeLastVertex(edge.First.ComponentId);
    removeLastVertex(edge.Second.ComponentId);

    {
        auto firstComponent = edge.First.ComponentId;
        auto secondComponent = edge.Second.ComponentId;
        if (firstComponent >= secondComponent) {
            std::swap(firstComponent, secondComponent);
        }

        if (newComponents[firstComponent] == 0) {
            newComponents.erase(newComponents.begin() + firstComponent);
            --secondComponent;
        }

        if (newComponents[secondComponent] == 0) {
            newComponents.erase(newComponents.begin() + secondComponent);
        }
    }

    auto newCompleteGraph = std::make_unique<TCompleteGraph>(std::move(newComponents));
    TDenseGraph newGraph(*newCompleteGraph, finalEdgeSet);
    return {newGraph, std::move(newCompleteGraph)};
}

TEdgeSet TDenseGraph::SwapVerticesInSet(const TEdgeSet& edgeSet, size_t componentId, size_t firstVertex, size_t secondVertex) {
    TVertex first(componentId, firstVertex);
    TVertex second(componentId, secondVertex);

    TEdgeSet newEdgeSet;
    for (const auto& edge : edgeSet) {
        if (edge.First == first) {
            newEdgeSet.emplace(second, edge.Second);
        } else if (edge.Second == first) {
            newEdgeSet.emplace(edge.First, second);
        } else if (edge.First == second) {
            newEdgeSet.emplace(first, edge.Second);
        } else if (edge.Second == second) {
            newEdgeSet.emplace(edge.First, first);
        } else {
            newEdgeSet.emplace(edge);
        }
    }

    return newEdgeSet;
}

INT TDenseGraph::ComponentSize(size_t component) const {
    return Graph->ComponentSize(component);
}

size_t TDenseGraph::ComponentsNumber() const {
    return Graph->ComponentsNumber();
}
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
