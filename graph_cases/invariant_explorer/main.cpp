#include "optparser/optparser.h"
#include "executer/executer.h"
#include "math_utils/combinatorics.h"
#include "multipartite_graphs/multipartite_graphs.h"
#include "multithread_writer/writer.h"

#include <iostream>
#include <mutex>
#include <algorithm>
#include <functional>
#include <limits>

struct TOptions {
    NMultipartiteGraphs::TCompleteGraph Graph;
    unsigned int MaxNumberOfEdges;
    std::string Invariant;
    int ThreadCount;
    size_t MaxQueueSize;

    static TOptions ParseFromCommandLine(int argc, const char ** argv) {
        TOptions opts{};

        std::vector<INT> graph;

        TParser parser;
        parser.AddFreeArgument("graph")
            .Required(true)
            .AppendTo(&graph);

        parser.AddLongOption('e', "max-edge-number")
            .Default("0")
            .Store(&opts.MaxNumberOfEdges);

        parser.AddLongOption('i', "invariant")
            .Default("i4")
            .Store(&opts.Invariant);

        parser.AddLongOption('t', "thread-count")
            .Default("1")
            .Store(&opts.ThreadCount);

        parser.AddLongOption("max-queue-size")
            .Default("1000")
            .Store(&opts.MaxQueueSize);

        parser.Parse(argc, argv);

        opts.Graph = {graph.begin(), graph.end()};

        return opts;
    }
};


template<typename TNumber>
struct TResult {
    unsigned int NumberOfEdges;
    TNumber MaxValue = std::numeric_limits<TNumber>::min();
    TNumber MinValue = std::numeric_limits<TNumber>::max();

    TResult(unsigned int numberOfEdges)
        : NumberOfEdges(numberOfEdges)
        , MaxValue{std::numeric_limits<TNumber>::min()}
        , MinValue{std::numeric_limits<TNumber>::max()}
    {
    }
};

template<typename TNumber>
class TResultCollector {
public:
    TResultCollector(unsigned int numberOfEdges)
        : Result(numberOfEdges)
        , Mutex{}
    {
    }

    inline TResult<TNumber> GetResult() const {
        return Result;
    }

    TResultCollector& Add(unsigned int value) {
        std::unique_lock<std::mutex> lock{Mutex};
        Result.MaxValue = std::max(Result.MaxValue, value);
        Result.MinValue = std::min(Result.MinValue, value);
        return *this;
    }

private:
    TResult<TNumber> Result;
    std::mutex Mutex;
};

template<typename TNumber>
struct TTask : public ITask {
    using TInvariant = NMultipartiteGraphs::TInvariant<TNumber>;

    TTask(NMultipartiteGraphs::TDenseGraph&& graph, const TInvariant& invariant, TResultCollector<TNumber>* collector)
        : Graph(graph)
        , Collector(collector)
        , Invariant(invariant)
    {
    }

    void Do() override {
        Collector->Add(Invariant(Graph));
    }

    NMultipartiteGraphs::TDenseGraph Graph;
    TResultCollector<TNumber>* Collector;
    TInvariant Invariant;
};


template<typename TNumber>
std::deque<TResultCollector<TNumber>> CheckAllEdges(const NMultipartiteGraphs::TCompleteGraph& graph, unsigned int maxNumberOfEdges, const typename TTask<TNumber>::TInvariant& invariant, IExecuter* executer) {
    auto allEdges = graph.GenerateAllEdges();
    std::deque<TResultCollector<TNumber>> collectors;
    for (unsigned int numberOfEdges = 1; numberOfEdges <= maxNumberOfEdges; ++numberOfEdges) {
        collectors.emplace_back(numberOfEdges);
        for (const auto& combination : TChoiceGenerator(allEdges.size(), numberOfEdges)) {
            NMultipartiteGraphs::TEdgeSet edgeSet;
            for (auto i : combination) {
                edgeSet.insert(allEdges[i]);
            }
            executer->Add(std::make_unique<TTask<TNumber>>(NMultipartiteGraphs::TDenseGraph{graph, std::move(edgeSet)}, invariant, &collectors.back()));
        }
    }

    return collectors;
}


TTask<unsigned int>::TInvariant MakeInvariant(const std::string& name) {
    if (name == "i4") {
        return &NMultipartiteGraphs::IGraph::I4Invariant;
    }

    if (name == "pt") {
        return &NMultipartiteGraphs::IGraph::PtInvariant;
    }

    throw std::logic_error("unknown invariant: " + name);
}

int main(int argc, const char ** argv) {
    auto options = TOptions::ParseFromCommandLine(argc, argv);
    const auto& graph = options.Graph;
    auto executer = CreateExecuter(options.ThreadCount, options.MaxQueueSize, nullptr);
    unsigned int maxNumberOfEdges = (options.MaxNumberOfEdges == 0) ? graph.I2Invariant() : options.MaxNumberOfEdges;
    auto collectors = CheckAllEdges<unsigned int>(graph, maxNumberOfEdges, MakeInvariant(options.Invariant), executer.get());
    executer->Stop();

    for (const auto& collector : collectors) {
        auto result = collector.GetResult();
        std::cout << result.NumberOfEdges << " " << result.MinValue << " " << result.MaxValue << std::endl;
    }
}