#include "multipartite_graphs.h"
#include "math_utils.h"

#include <string>
#include <iostream>
#include <sstream>

class TTestRegistry;
TTestRegistry* GetRegistry();

class ITest {
public:
    ITest();
    virtual void Run() const = 0;
    virtual ~ITest() = default;
    std::string GetName() {
        auto prefix = GetPrefix();
        if (prefix.empty()) {
            return GetLocalName();
        }

        return prefix + "::" + GetLocalName();
    };
    virtual std::string GetPrefix() {
        return "";
    };

    virtual std::string GetLocalName() const = 0;
};


class TTestFail : public std::exception {
private:
    std::string Message;
public:
    TTestFail(std::string message)
        : Message(std::move(message))
    {
    }

    const char * what() const noexcept override {
        return Message.c_str();
    }
};

#define FAIL(msg) throw TTestFail(msg)
#define ASSERT(__cond, __msg) do {\
    if (!(__cond)) {\
        FAIL((std::stringstream() << "Condition " << #__cond << " failed: " << __msg).str()); \
    } \
} while(0)


struct TTestRunStat {
    int Success = 0;
    int Failed = 0;
    int Crashed = 0;
};

class TTestRegistry {
public:
    TTestRegistry() = default;

    TTestRegistry& Add(ITest* test) {
        Tests.push_back(test);
        return *this;
    }

    void RunAll() {
        TTestRunStat stat;
        for (auto test: Tests) {
            Run(test, stat);
        }

        std::cerr << "**************************" << std::endl;
        std::cerr << " * Success: " << stat.Success << std::endl;
        std::cerr << " * Failed: " << stat.Failed << std::endl;
        std::cerr << " * Crashed: " << stat.Crashed << std::endl;
    }

    void Run(ITest* test, TTestRunStat& stat) {
        try {
            test->Run();
            std::cerr << test->GetName() << " OK\n";
            stat.Success++;
        } catch (const TTestFail& fail) {
            std::cerr << test->GetName() << " Failed: " << fail.what() << std::endl;
            stat.Failed++;
        } catch (...) {
            std::cerr << test->GetName() << " Crashed " << std::endl;
            stat.Crashed++;
        }
    }

private:
    std::vector<ITest*> Tests;
};

ITest::ITest() {
    GetRegistry()->Add(this);
}

TTestRegistry* GetRegistry() {
    static TTestRegistry* registry;
    if (!registry) {
        registry = new TTestRegistry();
    }

    return registry;
}

#define UNIT_TEST(name) class TTest##name : public ITest {\
public: \
    std::string Name; \
    TTest##name() \
        : Name(#name) \
    { \
    } \
    void Run() const override; \
    std::string GetLocalName() const override { return Name;} \
}; \
TTest##name test##name{};    \
    void TTest##name::Run() const

#define UNIT_TEST_SUITE(name) namespace NTestSuite##name {\
    class ITest : public ::ITest { \
    public: \
        ITest() : ::ITest() {}; \
        virtual ~ITest() = default; \
        std::string GetPrefix() override { \
            return #name; \
        } \
    }; \
}\
namespace NTestSuite##name

UNIT_TEST(Simple) {
    using namespace NMultipartiteGraphs;

    TEdgeSet edges = {
        TEdge(TVertex(0, 1), TVertex(1, 0))
    };

    auto completeGraph = TCompleteGraph({3, 3, 3});
    TDenseGraph graph(completeGraph, edges);

    ASSERT(completeGraph.I2Invariant() == 27, "i2 invariant mismatched");
    ASSERT(completeGraph.I3Invariant() == 27, "i2 invariant mismatched");
    ASSERT(completeGraph.I4Invariant() == 27, "i2 invariant mismatched");

    ASSERT(graph.I2Invariant() == 26, "i2 invariant mismatched");
    ASSERT(graph.I3Invariant() == 24, "i3 invariant mismatched");
    ASSERT(graph.I4Invariant() == 26, "i4 invariant mismatched");
}

UNIT_TEST_SUITE(Sigma) {
    UNIT_TEST(Iterators) {
        std::vector<int> v = {1, 2, 3, 4};
        ASSERT(Sigma(1, v.begin(), v.end()) == 10, "sigma1 mismatched");
        ASSERT(Sigma(2, v.begin(), v.end()) == 35, "sigma2 mismatched");
        ASSERT(Sigma(3, v.begin(), v.end()) == 50, "sigma3 mismatched");
        ASSERT(Sigma(4, v.begin(), v.end()) == 24, "sigma4 mismatched");
    }

    UNIT_TEST(Container) {
        std::vector<int> v = {1, 2, 3, 4};
        ASSERT(Sigma(1, v) == 10, "sigma1 mismatched");
        ASSERT(Sigma(2, v) == 35, "sigma2 mismatched");
        ASSERT(Sigma(3, v) == 50, "sigma3 mismatched");
        ASSERT(Sigma(4, v) == 24, "sigma4 mismatched");
    }

    UNIT_TEST(Container2) {
        std::vector<int> v = {1, 2, 3, 4, 5};
        ASSERT(Sigma(1, v) == 15, "sigma1 mismatched");
        ASSERT(Sigma(2, v) == 85, "sigma2 mismatched");
        ASSERT(Sigma(3, v) == 225, "sigma3 mismatched");
        ASSERT(Sigma(4, v) == 274, "sigma4 mismatched");
    }
}

UNIT_TEST_SUITE(PairGenerator) {
    UNIT_TEST(Simple) {
        TPairGenerator generator(3);
        std::vector<std::pair<size_t, size_t>> answers = {
            {0, 1},
            {0, 2},
            {1, 2},
        };

        size_t i = 0;
        for (auto pair : generator) {
            ASSERT(pair == answers[i], (std::stringstream() << "iteration " << i << "failed").str());
            ++i;
        }

        ASSERT(i == answers.size(), "wrong number if iterations");
    }
}

template<typename T>
void AssertVectors(const std::vector<T>& left, const std::vector<T>& right) {
    ASSERT(left.size() == right.size(), "sizes mismatch");
    for (size_t i = 0; i != left.size(); ++i) {
        ASSERT(left[i] == right[i], (std::stringstream() << "failed at " << i).str());
    }
}


UNIT_TEST_SUITE(CombinationGenerator) {
    UNIT_TEST(MaxCombination) {
        AssertVectors({1, 2, 3, 4}, TChoiceGenerator::GenerateMaxCombination(5, 4));
    }

    UNIT_TEST(Simple) {
        std::vector<std::vector<size_t>> answers = {
            {0, 1, 2, 3},
            {0, 1, 2, 4},
            {0, 1, 3, 4},
            {0, 2, 3, 4},
            {1, 2, 3, 4}
        };

        size_t iter = 0;
        for (const auto& choice : TChoiceGenerator(5, 4)) {
            AssertVectors(answers[iter], choice);
            ++iter;
        }

        ASSERT(iter == answers.size(), "wrong number of iteration");
    }
}

UNIT_TEST(BetweenParts) {
   using namespace NMultipartiteGraphs;
   TCompleteGraph graph({3, 2});
   std::vector<TEdge> answer = {
       TEdge(TVertex(0, 0), TVertex(1, 0)),
       TEdge(TVertex(0, 0), TVertex(1, 1)),
       TEdge(TVertex(0, 1), TVertex(1, 0)),
       TEdge(TVertex(0, 1), TVertex(1, 1)),
       TEdge(TVertex(0, 2), TVertex(1, 0)),
       TEdge(TVertex(0, 2), TVertex(1, 1)),
   };

   AssertVectors(answer, graph.GenerateEdgesBetweenComponents(0, 1));
}

int main() {
    GetRegistry()->RunAll();
}
