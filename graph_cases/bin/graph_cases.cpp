#include <fstream>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

#include "executer/executer.h"
#include "local_types.h"
#include "math_utils/combinatorics.h"
#include "multipartite_graphs/multipartite_graphs.h"
#include "multithread_writer/writer.h"
#include "optparser/optparser.h"
#include "utils/print.h"


void WriteEdgeStat(size_t componentsNumber, const NMultipartiteGraphs::TEdgeSet& edgeSet, std::ostream& outp) {
    std::vector<std::vector<int>> edgeStat(componentsNumber, std::vector<int>(componentsNumber, 0));

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


struct TInvariantChecker {
    using TChecker = NMultipartiteGraphs::TInvariant<INT>;

    std::string Name;
    TChecker Checker;

    TInvariantChecker() = default;

    TInvariantChecker(std::string name, TChecker checker) noexcept
        : Name(std::move(name))
        , Checker(std::move(checker))
    {
    }
};

static TInvariantChecker checkers[] = {
    {"I3", &NMultipartiteGraphs::IGraph::I3Invariant},
    {"I4", &NMultipartiteGraphs::IGraph::I4Invariant},
    {"PT", &NMultipartiteGraphs::IGraph::PtInvariant},
    {"Acyclic", &NMultipartiteGraphs::IGraph::CountAcyclicOrientations},
};

struct TCompareOptions {
    bool ComputeAll = true;
    bool WriteEdgeSet = true;
};

void CompareSourceAndDense(const NMultipartiteGraphs::TCompleteGraph& source, const NMultipartiteGraphs::TDenseGraph& target, std::ostream& outp, const TCompareOptions& options) {
    if (options.WriteEdgeSet) {
        PrintCollection(outp, target.DeletedEdges());
    }

    const std::string* reason = nullptr;
    for (const auto& checker : checkers) {
        auto sourceValue = checker.Checker(source);
        auto targetValue = checker.Checker(target);
        outp << checker.Name << ": " << targetValue << ' ';
        if (sourceValue != targetValue) {
            if (reason == nullptr) {
                reason = &checker.Name;
            }

            if (!options.ComputeAll) {
                break;
            }
        }
    }

    if (reason == nullptr) {
        outp << "Answer: YES";
    } else {
        outp << "Answer: NO Reason: " << *reason;
    }

    outp << ' ';
    WriteEdgeStat(source.ComponentsNumber(), target.DeletedEdges(), outp);
    outp << "\n";
}

class TCompareGraphsTask : public ITask {
public:
    TCompareGraphsTask(const NMultipartiteGraphs::TCompleteGraph& source, TWriter& writer, NMultipartiteGraphs::TDenseGraph target, TCompareOptions options)
        : Source(source)
        , Target(std::move(target))
        , Writer(writer)
        , Options(options)
    {
    }

    void Do() override {
        std::stringstream ss;
        CompareSourceAndDense(Source, Target, ss, Options);
        ss.flush();
        Writer.Push(ss.str());
    }

private:
    const NMultipartiteGraphs::TCompleteGraph& Source;
    NMultipartiteGraphs::TDenseGraph Target;
    TWriter& Writer;
    TCompareOptions Options;
};


void compare_two_graphs(const NMultipartiteGraphs::TCompleteGraph& source, const NMultipartiteGraphs::TCompleteGraph& target,
                        std::ostream& debug, int threadCount, const TCompareOptions& options) {
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

    for (const auto& checker: checkers) {
        debug << "Source " << checker.Name << ": "  << checker.Checker(source) << std::endl;
        debug << "Target " << checker.Name << ": "  << checker.Checker(target) << std::endl;
    }

    debug << std::flush;

    std::vector<NMultipartiteGraphs::TEdge> allEdges = target.GenerateAllEdges();

    TWriter writer{debug};

    auto executer = CreateExecuter(threadCount, 1000, nullptr);

    size_t done = 0;
    for (const auto& combination : TChoiceGenerator(allEdges.size(), edge_diff)) {
        NMultipartiteGraphs::TEdgeSet current_edges;
        for (const auto x: combination) {
            current_edges.insert(allEdges[x]);
        }

        NMultipartiteGraphs::TDenseGraph newTarget{target, std::move(current_edges)};
        executer->Add(std::make_unique<TCompareGraphsTask>(source, writer, std::move(newTarget), options));
        done += 1;
        if (done % 100000 == 0) {
            std::cerr << "done: " << done << ", queue size: " << executer->Size() << std::endl;
        }
    }

    std::cerr << "all pushed" << std::endl;
    executer->Stop();
}

struct TOptions {
    std::vector<INT> Source;
    std::vector<INT> Target;
    int ThreadCount = 1;
    std::string OutputFile;

    TCompareOptions Options;

    static TOptions Parse(int argc, const char ** argv) {
        TOptions opts;

        TParser parser;
        parser.AddShortOption('s').AppendTo(&opts.Source).Required(true);
        parser.AddShortOption('t').AppendTo(&opts.Target).Required(true);
        parser.AddLongOption("thread-count").Store(&opts.ThreadCount).Default("6");
        parser.AddLongOption("output-file").Store(&opts.OutputFile).Default("");
        parser.AddLongOption("compute-all").SetFlag(&opts.Options.ComputeAll).Default("false");
        parser.AddLongOption("write-all-edges").SetFlag(&opts.Options.WriteEdgeSet).Default("false");

        parser.Parse(argc, argv);

        return opts;
    }
};


int main(int argc, const char ** argv) {
    TOptions opts = TOptions::Parse(argc, argv);
    NMultipartiteGraphs::TCompleteGraph source(opts.Source.begin(), opts.Source.end());
    NMultipartiteGraphs::TCompleteGraph target(opts.Target.begin(), opts.Target.end());

    std::unique_ptr<std::ofstream> out(nullptr);
    if (!opts.OutputFile.empty()) {
        out = std::make_unique<std::ofstream>();
        out->open(opts.OutputFile);
    }

    compare_two_graphs(source, target, out ? *out : std::cout, opts.ThreadCount, opts.Options);
    if (out) {
        out->flush();
        out->close();
    }

    return 0;
}
