#include <utility>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>

#include "math_utils.h"
#include "multipartite_graphs.h"

typedef NMultipartiteGraphs::TEdge TEdge;
using TGraph = NMultipartiteGraphs::TCompleteGraph;
typedef std::unordered_set<TEdge> TEdgeSet;



template<class TContainer>
std::ostream& print_collection(std::ostream& outp, const TContainer& container) {
    for (const auto& x: container) {
        outp << x << " ";
    }
    return outp;
}

void compare_two_graphs(const TGraph& source, const TGraph& target, std::ostream& debug=std::cout) {
    debug << "Checking graphs" << std::endl;
    debug << "Source: " << source << std::endl;
    debug << "Target: " << target << std::endl;

    debug << "Checking vertices" << std::endl;
    INT source_vertices = source.VerticesCount();
    INT target_vertices = target.VerticesCount();

    debug << "Source: " << source_vertices << std::endl;
    debug << "Target: " << target_vertices << std::endl;
    if (source_vertices != target_vertices) {
        debug << "Check failed" << std::endl;
        debug << "Answer: No" << std::endl;
        debug << "Reason: Vertices numbers mismatches" << std::endl;
        return;
    } else {
        debug << "Check passed" << std::endl;
    }

    debug << "Checking edges" << std::endl;
    INT source_edges = source.I2Invariant();
    INT target_edges = target.I2Invariant();

    debug << "Source: " << source_edges << std::endl;
    debug << "Target: " << target_edges << std::endl;

    if (source_edges > target_edges) {
        debug << "Check failed" << std::endl;
        debug << "Answer: No" << std::endl;
        debug << "Reason: Source contains more edges" << std::endl;
        return;
    } else if (source_edges == target_edges) {
        if (source == target) {
            debug << "Check passed" << std::endl;
            debug << "Answer: Yes" << std::endl;
            debug << "Reason: Graphs are equal" << std::endl;
        } else {
            debug << "Check failed" << std::endl;
            debug << "Answer: No" << std::endl;
            debug << "Reason: Two different multipartite graphs" << std::endl;
        }
        return;
    }

    INT edge_diff = target_edges - source_edges;
    debug << "Difference: " << edge_diff << std::endl;
    assert(source.ComponentsNumber() == 3);

    INT source_i3 = source.I3Invariant();
    INT target_i3 = target.I3Invariant();

    INT source_i4 = source.I4Invariant();
    INT target_i4 = target.I4Invariant();

    debug << "Source I3: " << source_i3 << std::endl;
    debug << "Target I3: " << target_i3 << std::endl;
    debug << "Source I4: " << source_i4 << std::endl;
    debug << "Target I4: " << target_i4 << std::endl;
    debug << std::flush;

    std::vector<TEdge> edges12 = target.GenerateEdgesBetweenComponents(0, 1);
    std::vector<TEdge> edges13 = target.GenerateEdgesBetweenComponents(0, 2);
    std::vector<TEdge> edges23 = target.GenerateEdgesBetweenComponents(1, 2);

    std::vector<TEdge> all_edges;
    all_edges.reserve(target_edges);
    all_edges.insert(all_edges.end(), edges12.begin(), edges12.end());
    all_edges.insert(all_edges.end(), edges13.begin(), edges13.end());
    all_edges.insert(all_edges.end(), edges23.begin(), edges23.end());

    for (const auto& combination : TChoiceGenerator(all_edges.size(), edge_diff)) {
        TEdgeSet current_edges;
        for (const auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        assert(current_edges.size() == edge_diff);

        print_collection(debug, current_edges);
        NMultipartiteGraphs::TDenseGraph newTarget{target, current_edges};
        INT i3_new = newTarget.I3Invariant();
        debug << "I3:" << i3_new << " ";
        if (i3_new != source_i3) {
            debug << "Answer: No Reason: I3";
        } else {
            auto i4_new = newTarget.I4Invariant();
            debug << "I4: " << i4_new << " ";
            if (i4_new == source_i4) {
                debug << " Answer: Yes";
            } else {
                debug << " Answer: No Reason: I4";
            }
        }

        debug << " ";
        int c_e12 = 0;
        int c_e13 = 0;
        int c_e23 = 0;
        for (const auto& edge: current_edges) {
            auto firstComponent = edge.First.ComponentId;
            auto secondComponent = edge.Second.ComponentId;
            if (firstComponent > secondComponent) {
                std::swap(firstComponent, secondComponent);
            }

            if (firstComponent == 0) {
                if (secondComponent == 1) {
                    c_e12 += 1;
                } else {
                    c_e13 += 1;
                }
            } else {
                c_e23 += 1;
            }
        }

        debug << "e12=" << c_e12 << ", e23=" << c_e23 << ", e13=" << c_e13;
        debug << std::endl;
    }
}


int main(void) {
    TGraph graph{7, 2, 2};
    TGraph graph2{4, 4, 3};
    compare_two_graphs(graph, graph2);
    return 0;
}
