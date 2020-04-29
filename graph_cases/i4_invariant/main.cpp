#include "optparser/optparser.h"
#include "multipartite_graphs.h"
#include "executer/executer.h"
#include "multithread_writer/writer.h"

#include <iostream>
#include <mutex>
#include <algorithm>

struct TOptions {
    unsigned int V1;
    unsigned int V2;
    unsigned int MaxNumberOfEdges;
    int ThreadCount;
    size_t MaxQueueSize;

    static TOptions ParseFromCommandLine(int argc, const char ** argv) {
        TOptions opts{};

        TParser parser;
        parser.AddFreeArgument("v1")
            .Required(true)
            .Store(&opts.V1);

        parser.AddFreeArgument("v2")
            .Required(true)
            .Store(&opts.V2);

        parser.AddLongOption('e', "max-edge-number")
            .Default("0")
            .Store(&opts.MaxNumberOfEdges);

        parser.AddLongOption('t', "thread-count")
            .Default("1")
            .Store(&opts.ThreadCount);

        parser.AddLongOption("max-queue-size")
            .Default("1000")
            .Store(&opts.MaxQueueSize);

        parser.Parse(argc, argv);

        return opts;
    }
};


struct TResult {
    unsigned int NumberOfEdges;
    unsigned int MaxI4 = 0;
    unsigned int MinI4 = -1;

    TResult(unsigned int numberOfEdges)
        : NumberOfEdges(numberOfEdges)
        , MaxI4(0)
        , MinI4(-1)
    {
    }
};

class TResultCollector {
public:
    TResultCollector(unsigned int numberOfEdges)
        : Result(numberOfEdges)
        , Mutex{}
    {
    }

    inline TResult GetResult() const {
        return Result;
    }

    TResultCollector& Add(unsigned int i4) {
        std::unique_lock<std::mutex> lock{Mutex};
        Result.MaxI4 = std::max(Result.MaxI4, i4);
        Result.MinI4 = std::min(Result.MinI4, i4);
        return *this;
    }

private:
    TResult Result;
    std::mutex Mutex;
};

struct TTask : public ITask {
    TTask(NMultipartiteGraphs::TDenseGraph&& graph, TResultCollector* collector)
        : Graph(graph)
        , Collector(collector)
        {
        }

    void Do() override {
        Collector->Add(Graph.I4Invariant());
    }

    NMultipartiteGraphs::TDenseGraph Graph;
    TResultCollector* Collector;
};


std::deque<TResultCollector> CheckAllEdges(const NMultipartiteGraphs::TCompleteGraph& graph, unsigned int maxNumberOfEdges, IExecuter* executer) {
    auto allEdges = graph.GenerateAllEdges();
    std::deque<TResultCollector> collectors;
    for (unsigned int numberOfEdges = 1; numberOfEdges <= maxNumberOfEdges; ++numberOfEdges) {
        collectors.emplace_back(numberOfEdges);
        for (const auto& combination : TChoiceGenerator(allEdges.size(), numberOfEdges)) {
            NMultipartiteGraphs::TEdgeSet edgeSet;
            for (auto i : combination) {
                edgeSet.insert(allEdges[i]);
            }
            executer->Add(std::make_unique<TTask>(NMultipartiteGraphs::TDenseGraph{graph, std::move(edgeSet)}, &collectors.back()));
        }
    }

    return collectors;
}

int main(int argc, const char ** argv) {
    auto options = TOptions::ParseFromCommandLine(argc, argv);
    NMultipartiteGraphs::TCompleteGraph graph{{options.V1, options.V2}};

    auto executer = CreateExecuter(options.ThreadCount, options.MaxQueueSize, nullptr);
    auto collectors = CheckAllEdges(graph, (options.MaxNumberOfEdges == 0) ? graph.I2Invariant() : options.MaxNumberOfEdges, executer.get());
    executer->Stop();

    for (const auto& collector : collectors) {
        TResult result = collector.GetResult();
        std::cout << result.NumberOfEdges << " " << result.MinI4 << " " << result.MaxI4 << std::endl;
    }
}