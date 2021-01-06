#include "multipartite_graphs/multipartite_graphs.h"
#include "optparser/optparser.h"

#include <iostream>
#include <vector>
#include <charconv>
#include <memory>
#include <fstream>


class IDataAsker {
public:
    IDataAsker() = default;

    virtual void AskCompleteGraph(std::ostream& output) const = 0;

    virtual void AskEdgeCount(std::ostream& output) const = 0;

    virtual void AskEdge(std::ostream& output) const = 0;

    virtual ~IDataAsker() = default;
};

class IDataWriter {
public:
    IDataWriter() = default;

    virtual void WriteAnswer(const NMultipartiteGraphs::TDenseGraph& graph, long long answer, std::ostream& output) const = 0;

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


bool RunOne(std::istream& input, std::ostream& output, const IDataAsker& asker, const IDataWriter& writer) {
    asker.AskCompleteGraph(output);
    std::string completeGraphLine;
    std::getline(input, completeGraphLine);

    if (completeGraphLine == "exit") {
        return false;
    }

    auto completeGraph = ParseCompleteGraph(completeGraphLine);

    unsigned edgeCount;
    asker.AskEdgeCount(output);
    input >> edgeCount;

    NMultipartiteGraphs::TEdgeSet edges;
    for (unsigned edgeNum = 0; edgeNum != edgeCount; ++edgeNum) {
        asker.AskEdge(output);
        std::string line;
        std::getline(input, line);
        while (line.empty()) {
            std::getline(input, line);
        }
        auto tokens = ReadLine<int>(line);
        while (tokens.size() != 4) {
            output << "expect four numbers (component, index, component, index), got"  << line;
            std::getline(input, line);
            tokens = ReadLine<int>(line);
        }

        NMultipartiteGraphs::TVertex first(tokens[0], tokens[1]);
        NMultipartiteGraphs::TVertex second(tokens[2], tokens[3]);
        edges.emplace(first, second);
    }

    NMultipartiteGraphs::TDenseGraph denseGraph(completeGraph, std::move(edges));
    writer.WriteAnswer(denseGraph, denseGraph.CountAcyclicOrientations(), output);

    return true;
}

class TSilentAsker : public IDataAsker {
public:
    void AskCompleteGraph(std::ostream&) const override {

    }

    void AskEdgeCount(std::ostream&) const override {

    }

    void AskEdge(std::ostream&) const override {

    }

    virtual ~TSilentAsker() = default;
};

class TFullWriter : public IDataWriter {
public:
    void WriteAnswer(const NMultipartiteGraphs::TDenseGraph& graph, long long answer, std::ostream& output) const override {
       output << (*graph.BaseGraph());
       output << " {";
       bool first = true;
       for (const auto& edge : graph.DeletedEdges()) {
           if (!first) {
               output << ", ";
           }
           output << edge;
           first = false;
       }

       output << "} ";
       output << answer;
       output << std::endl;
    }

    virtual ~TFullWriter() = default;
};


class TVerboseAsker : public IDataAsker {
public:
    void AskCompleteGraph(std::ostream& output) const override {
        output << "Enter compete graph, separating partitions with space" << std::endl;
    }

    void AskEdgeCount(std::ostream& output) const override {
        output << "Enter the number of deleted edges: ";
    }

    void AskEdge(std::ostream& output) const override {
        output << "Input edge: ";
    }

    virtual ~TVerboseAsker() = default;
};

class TSimpleWriter : public IDataWriter {
public:
    void WriteAnswer(const NMultipartiteGraphs::TDenseGraph&, long long int answer, std::ostream &output) const override {
        output << answer << std::endl;
    }

    virtual ~TSimpleWriter() = default;
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

    std::unique_ptr<IDataAsker> asker;
    if (inputFile.empty()) {
        asker = std::make_unique<TVerboseAsker>();
    } else {
        asker = std::make_unique<TSilentAsker>();
    }
    std::unique_ptr<IDataWriter> writer;
    if (outputFile.empty()) {
        writer = std::make_unique<TSimpleWriter>();
    }else {
        writer = std::make_unique<TFullWriter>();
    }

    while (RunOne(inputStream, outputStream, *asker, *writer)) {
    }

    if (inputFileStream.get() != nullptr) {
        inputFileStream->close();
    }

    if (outputFileStream.get() != nullptr) {
        outputFileStream->close();
    }

    return 0;
}
