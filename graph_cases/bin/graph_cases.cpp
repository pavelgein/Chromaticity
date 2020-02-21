#include <utility>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>

#include "local_types.h"
#include "multipartite_graphs.h"
#include "utils/print.h"

typedef NMultipartiteGraphs::TEdge TEdge;
using TGraph = NMultipartiteGraphs::TCompleteGraph;
typedef std::unordered_set<TEdge> TEdgeSet;


void WriteEdgeStat(size_t componentsNumber, const TEdgeSet& edgeSet, std::ostream& outp) {
    std::vector<std::vector<int>> edgeStat{};
    for (size_t i = 0; i != componentsNumber; ++i) {
        edgeStat.push_back(std::vector<int>(componentsNumber, 0));
    }

    for (const auto& edge: edgeSet) {
        auto firstComponent = edge.First.ComponentId;
        auto secondComponent = edge.Second.ComponentId;
        if (firstComponent > secondComponent) {
            std::swap(firstComponent, secondComponent);
        }

        edgeStat[firstComponent][secondComponent] += 1;
    }

    bool first = true;
    for (size_t i = 0; i + 1 != edgeStat.size(); ++i) {
        for (size_t j = i + 1; j != edgeStat.size(); ++j) {
            if (!first) {
                outp << ", ";
            }
            outp << 'e' << i + 1 << j + 1 << '=' << edgeStat[i][j];
            first = false;
        }
    }
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

    INT source_i3 = source.I3Invariant();
    INT target_i3 = target.I3Invariant();

    INT source_i4 = source.I4Invariant();
    INT target_i4 = target.I4Invariant();

    debug << "Source I3: " << source_i3 << std::endl;
    debug << "Target I3: " << target_i3 << std::endl;
    debug << "Source I4: " << source_i4 << std::endl;
    debug << "Target I4: " << target_i4 << std::endl;
    debug << std::flush;

    std::vector<TEdge> all_edges = target.GenerateAllEdges();

    for (const auto& combination : TChoiceGenerator(all_edges.size(), edge_diff)) {
        TEdgeSet current_edges;
        for (const auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        PrintCollection(debug, current_edges);
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
        WriteEdgeStat(source.ComponentsNumber(), current_edges, debug);

        debug << std::endl;
    }
}


int main(void) {
    TGraph graph{7, 2, 2};
    TGraph graph2{4, 4, 3};
    compare_two_graphs(graph, graph2);
    return 0;
}
