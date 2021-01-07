#include "multipartite_graphs/multipartite_graphs.h"
#include "optparser/optparser.h"

#include <iostream>
#include <vector>
#include <charconv>
#include <memory>
#include <fstream>
#include <exception>


class IDataAsker {
public:
    IDataAsker() = default;

    virtual NMultipartiteGraphs::TCompleteGraph AskCompleteGraph() = 0;

    virtual unsigned AskEdgeCount() = 0;

    virtual NMultipartiteGraphs::TEdge AskEdge() = 0;

    virtual ~IDataAsker() = default;
};

class IDataWriter {
public:
    IDataWriter() = default;

    virtual void WriteAnswer(const NMultipartiteGraphs::TDenseGraph& graph, long long answer) const = 0;

    virtual ~IDataWriter() = default;
};

template <typename TNumber>
std::vector<TNumber> ReadLine(const std::string& line) {
    std::vector<TNumber> parts;
    size_t firstIndex = 0;
    while (firstIndex < line.size()) {
        size_t secondIndex = firstIndex + 1;
        while (secondIndex < line.size() && line[secondIndex] != ' ') {
            ++secondIndex;
        }

        TNumber part;
        if (auto result = std::from_chars(line.data() + firstIndex, line.data() + secondIndex, part); result.ec == std::errc::invalid_argument) {
            throw std::logic_error(std::string("bad token: ") + std::string(line.data() + firstIndex, line.data() + secondIndex));
        }

        parts.push_back(part);
        firstIndex = secondIndex + 1;
    }

    return parts;
}


NMultipartiteGraphs::TCompleteGraph ParseCompleteGraph(const std::string& line) {
    return NMultipartiteGraphs::TCompleteGraph(ReadLine<INT>(line));
}

class TStopProcessException : public std::exception {
};


bool RunOne(IDataAsker& asker, IDataWriter& writer) {
    try {
        NMultipartiteGraphs::TCompleteGraph completeGraph = asker.AskCompleteGraph();
        unsigned edgeCount = asker.AskEdgeCount();
        NMultipartiteGraphs::TEdgeSet edges;
        for (unsigned edgeNum = 0; edgeNum != edgeCount; ++edgeNum) {
            edges.emplace(asker.AskEdge());
        }

        NMultipartiteGraphs::TDenseGraph denseGraph(completeGraph, std::move(edges));
        writer.WriteAnswer(denseGraph, denseGraph.CountAcyclicOrientations());
    } catch (const TStopProcessException&) {
        return false;
    }

    return true;
}


std::string GetNonEmptyLine(std::istream& input) {
    std::string line;
    while (line.empty()) {
        std::getline(input, line);
    }

    return line;
}

class TStreamAsker : public IDataAsker {
public:
    TStreamAsker(std::istream& input, std::ostream& output, bool verbose)
        : Input(input)
        , Output(output)
        , Verbose(verbose)
    {
    }

public:
    NMultipartiteGraphs::TCompleteGraph AskCompleteGraph() override {
        if (Verbose) {
            Output << "Enter compete graph, separating partitions with space: ";
        }

        std::string line = GetNonEmptyLine();
        if (IsStopLine(line)) {
            throw TStopProcessException();
        }

        return ParseCompleteGraph(line);
    }

    unsigned int AskEdgeCount() override {
        if (Verbose) {
            Output << "Enter the number of deleted edges: ";
        }

        unsigned answer;
        Input >> answer;
        return answer;
    }

    NMultipartiteGraphs::TEdge AskEdge() override {
        if (Verbose) {
            Output << "Input edge: expect four numbers (component, index, component, index)";
        }

        auto tokens = ReadLine<int>(GetNonEmptyLine());
        while (tokens.size() != 4) {
            if (Verbose) {
                Output << "expect four numbers (component, index, component, index)" ;
            }
            tokens = ReadLine<int>(GetNonEmptyLine());
        }

        NMultipartiteGraphs::TVertex first(tokens[0], tokens[1]);
        NMultipartiteGraphs::TVertex second(tokens[2], tokens[3]);
        return {first, second};
    }

private:
    std::string GetNonEmptyLine() {
        return ::GetNonEmptyLine(Input);
    }

    static bool IsStopLine(const std::string& line) {
        return line == "exit";
    }

    std::istream& Input;
    std::ostream& Output;
    bool Verbose;
};

class TFullWriter : public IDataWriter {
public:
    TFullWriter(std::ostream& output)
        : Output(output)
    {
    }

    void WriteAnswer(const NMultipartiteGraphs::TDenseGraph& graph, long long answer) const override {
       Output << (*graph.BaseGraph());
       Output << " {";
       bool first = true;
       for (const auto& edge : graph.DeletedEdges()) {
           if (!first) {
               Output << ", ";
           }
           Output << edge;
           first = false;
       }

       Output << "} ";
       Output << answer;
       Output << std::endl;
    }

    virtual ~TFullWriter() = default;

private:
    std::ostream& Output;
};

class TSimpleWriter : public IDataWriter {
public:
    TSimpleWriter(std::ostream& output)
        : Output(output)
    {
    }

    void WriteAnswer(const NMultipartiteGraphs::TDenseGraph&, long long int answer) const override {
        Output << answer << std::endl;
    }

private:
    std::ostream& Output;
};


int main(int argc, const char ** argv) {
    std::string inputFile = "";
    std::string outputFile = "";

    TParser optParser;
    optParser.AddLongOption('i', "input-file").Store(&inputFile);
    optParser.AddLongOption('o', "output-file").Store(&outputFile);
    optParser.Parse(argc, argv);

    std::unique_ptr<std::ifstream> inputFileStream{nullptr};
    if (!inputFile.empty()) {
        inputFileStream.reset(new std::ifstream(inputFile));
    }

    std::unique_ptr<std::ofstream> outputFileStream{nullptr};
    if (!outputFile.empty()) {
        outputFileStream.reset(new std::ofstream(outputFile));
    }

    std::istream& inputStream = (inputFileStream.get() == nullptr) ? std::cin : *inputFileStream;
    std::ostream& outputStream = (outputFileStream.get() == nullptr) ? std::cout : *outputFileStream;

    std::unique_ptr<TStreamAsker> asker = std::make_unique<TStreamAsker>(inputStream, outputStream, outputFileStream.get() == nullptr);
    std::unique_ptr<IDataWriter> writer;
    if (outputFile.empty()) {
        writer = std::make_unique<TSimpleWriter>(outputStream);
    } else {
        writer = std::make_unique<TFullWriter>(outputStream);
    }

    while (RunOne(*asker, *writer)) {
    }

    if (inputFileStream.get() != nullptr) {
        inputFileStream->close();
    }

    if (outputFileStream.get() != nullptr) {
        outputFileStream->close();
    }

    return 0;
}
