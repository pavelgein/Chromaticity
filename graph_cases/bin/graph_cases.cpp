#include <utility>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <sstream>

#include "local_types.h"
#include "multipartite_graphs.h"
#include "utils/print.h"
#include "queue/queue.h"

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

void CompareSourceAndDense(const NMultipartiteGraphs::TCompleteGraph& source, const NMultipartiteGraphs::TDenseGraph& target, std::ostream& outp) {
    PrintCollection(outp, target.DeletedEdges());
    INT i3_new = target.I3Invariant();
    outp << "I3:" << i3_new << " ";
    if (i3_new != source.I3Invariant()) {
        outp << "Answer: No Reason: I3";
    } else {
        auto i4_new = target.I4Invariant();
        outp << "I4: " << i4_new << " ";
        if (i4_new == source.I4Invariant()) {
            outp << " Answer: Yes";
        } else {
            outp << " Answer: No Reason: I4";
        }
    }

    outp << " ";
    WriteEdgeStat(source.ComponentsNumber(), target.DeletedEdges(), outp);
    outp << std::endl;
}


void thread_func(const TGraph& source, std::ostream& outp, TMultiThreadQueue<NMultipartiteGraphs::TDenseGraph>& queue, std::atomic<bool>& alive) {
    NMultipartiteGraphs::TDenseGraph target;
    while (alive) {
        if (queue.Pop(std::chrono::seconds(1), target)) {
            std::stringstream ss;
            CompareSourceAndDense(source, target, ss);
            outp << ss.str();
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    std::cerr << "exit from" << std::this_thread::get_id() << std::endl;
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

    TMultiThreadQueue<NMultipartiteGraphs::TDenseGraph> queue(1000);
    std::atomic<bool> alive = true;
    std::vector<std::thread> workers;
    for (size_t i = 0; i != 6; ++i) {
        workers.emplace_back(thread_func, std::cref(source), std::ref(debug), std::ref(queue), std::ref(alive));
    }

    size_t done = 0;
    for (const auto& combination : TChoiceGenerator(all_edges.size(), edge_diff)) {
        TEdgeSet current_edges;
        for (const auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        NMultipartiteGraphs::TDenseGraph newTarget{target, current_edges};
        queue.Push(std::move(newTarget));
        done += 1;
        if (done % 100000 == 0) {
            std::cerr << "done: " << done << ", queue size: " << queue.Size() << std::endl;
        }
    }

    std::cerr << "all pushed" << std::endl;

    std::mutex m;
    std::unique_lock<std::mutex> lock(m);
    auto& cond = queue.Empty();
    cond.wait(lock);
    alive = false;

    for (auto& worker : workers) {
        worker.join();
    }
}

int main(void) {
    TGraph graph{7, 2, 2};
    TGraph graph2{4, 4, 3};
    compare_two_graphs(graph, graph2);
    return 0;
}
