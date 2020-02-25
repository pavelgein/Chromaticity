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

void WriteEdgeStat(size_t componentsNumber, const NMultipartiteGraphs::TEdgeSet& edgeSet, std::ostream& outp) {
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
    auto i4_new = target.I4Invariant();

    outp << "I3: " << i3_new << " I4: " << i4_new;
    if (i3_new != source.I3Invariant()) {
        outp << " Answer: No Reason: I3";
    } else {
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


void thread_func(const NMultipartiteGraphs::TCompleteGraph& source, std::ostream& outp, TMultiThreadQueue<NMultipartiteGraphs::TDenseGraph>& queue, std::atomic<bool>& alive) {
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

    std::stringstream msg;
    msg << "exit from" << std::this_thread::get_id() << std::endl;
    std::cerr << msg.str();
}

void compare_two_graphs(const NMultipartiteGraphs::TCompleteGraph& source, const NMultipartiteGraphs::TCompleteGraph& target,
                        std::ostream& debug, int threadCount) {
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

    std::vector<NMultipartiteGraphs::TEdge> all_edges = target.GenerateAllEdges();

    TMultiThreadQueue<NMultipartiteGraphs::TDenseGraph> queue(1000);
    std::atomic<bool> alive = true;
    std::vector<std::thread> workers;
    for (int i = 0; i != threadCount; ++i) {
        workers.emplace_back(thread_func, std::cref(source), std::ref(debug), std::ref(queue), std::ref(alive));
    }

    size_t done = 0;
    for (const auto& combination : TChoiceGenerator(all_edges.size(), edge_diff)) {
        NMultipartiteGraphs::TEdgeSet current_edges;
        for (const auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        NMultipartiteGraphs::TDenseGraph newTarget{target, std::move(current_edges)};
        queue.Push(std::move(newTarget));
        done += 1;
        if (done % 100000 == 0) {
            std::cerr << "done: " << done << ", queue size: " << queue.Size() << std::endl;
        }
    }

    std::cerr << "all pushed" << std::endl;

    queue.WaitEmpty();
    alive = false;
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

class TBadOptionException : public std::invalid_argument {
public:
    TBadOptionException(const char* msg)
        : std::invalid_argument(msg)
    {
    }

    TBadOptionException(const std::string& msg)
        : std::invalid_argument(msg)
    {
    }
};


struct TOptions {
    std::vector<INT> Source;
    std::vector<INT> Target;
    int ThreadCount = 6;

    static TOptions ParseCommandLineArguments(int argc, const char ** argv) {
        TOptions opts;
        int optNum = 1;
        while (optNum < argc) {
           const char * currentOption = argv[optNum];
           if (currentOption[0] != '-') {
               throw TBadOptionException("no free arguments");
           }

           if (strlen(currentOption) < 2) {
               throw TBadOptionException((std::stringstream() << "bad argument " << currentOption).str());
           }

           if (currentOption[1] == 's') {
               ++optNum;
               while (optNum < argc && argv[optNum][0] != '-') {
                   opts.Source.push_back( std::atoi(argv[optNum]));
                   optNum++;
               }
           } else if (currentOption[1] == 't') {
               ++optNum;
               while (optNum < argc && argv[optNum][0] != '-') {
                   opts.Target.push_back(std::atoi(argv[optNum]));
                   optNum++;
               }
           } else if (currentOption[1] == '-') {
               if (strcmp(currentOption, "--thread-count") == 0) {
                   optNum++;
                   if (optNum >= argc) {
                       throw TBadOptionException("expected number after thread-count");
                   }
                   opts.ThreadCount = std::atoi(argv[optNum]);
                   optNum++;
               } else {
                   throw TBadOptionException((std::stringstream() << "unknown option " << currentOption).str());
               }
           }
        }

        return opts;
    }
};

int main(int argc, const char ** argv) {
    TOptions opts = TOptions::ParseCommandLineArguments(argc, argv);
    NMultipartiteGraphs::TCompleteGraph source(opts.Source.begin(), opts.Source.end());
    NMultipartiteGraphs::TCompleteGraph target(opts.Target.begin(), opts.Target.end());
    compare_two_graphs(source, target, std::cout, opts.ThreadCount);
    return 0;
}
