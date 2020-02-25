#include "multipartite_graphs.h"
#include "math_utils.h"

#include "test_system/test_system.h"

#include "queue/queue.h"

#include <string>
#include <sstream>

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


UNIT_TEST_SUITE(TestAllEdges) {
    UNIT_TEST(TwoComponents) {
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

        AssertVectors(answer, graph.GenerateAllEdges());
    }

    UNIT_TEST(ThreeComponents) {
        using namespace NMultipartiteGraphs;
        TCompleteGraph graph({2, 2, 2});
        std::vector<TEdge> answer = {
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(0, 0), TVertex(1, 1)),
            TEdge(TVertex(0, 1), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 1)),
            TEdge(TVertex(0, 0), TVertex(2, 0)),
            TEdge(TVertex(0, 0), TVertex(2, 1)),
            TEdge(TVertex(0, 1), TVertex(2, 0)),
            TEdge(TVertex(0, 1), TVertex(2, 1)),
            TEdge(TVertex(1, 0), TVertex(2, 0)),
            TEdge(TVertex(1, 0), TVertex(2, 1)),
            TEdge(TVertex(1, 1), TVertex(2, 0)),
            TEdge(TVertex(1, 1), TVertex(2, 1)),
        };

        AssertVectors(answer, graph.GenerateAllEdges());
    }
}

UNIT_TEST_SUITE(Invariants) {
    UNIT_TEST(TestOneEdge) {
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

    UNIT_TEST(TestTwoEdges) {
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 1), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 1))
        };

        auto completeGraph = TCompleteGraph({3, 3, 3});
        TDenseGraph graph(completeGraph, edges);

        ASSERT(graph.I2Invariant() == 25, "i2 invariant mismatched");
        ASSERT(graph.I3Invariant() == 21, "i3 invariant mismatched");
        ASSERT(graph.I4Invariant() == 27, "i4 invariant mismatched");
    }

    UNIT_TEST(TestXi2) {
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 1), TVertex(1, 0)),
            TEdge(TVertex(1, 0), TVertex(2, 0))
        };

        auto completeGraph = TCompleteGraph({3, 3, 3});
        TDenseGraph graph(completeGraph, edges);

        ASSERT(graph.I2Invariant() == 25, "i2 invariant mismatched");
        ASSERT(graph.I3Invariant() == 22, "i3 invariant mismatched");
        ASSERT(graph.I4Invariant() == 21, "i4 invariant mismatched");
    }
}

UNIT_TEST_SUITE(Queue) {
    template<typename T>
    void thread_func(std::unordered_set<T>& set, std::atomic<bool>& alive, TMultiThreadQueue<T>& queue, std::mutex& m) {
        while (alive) {
            T result;
            if (queue.Pop(std::chrono::seconds(1), result)) {
                std::lock_guard<std::mutex> lock{m};
                set.insert(result);
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    template<typename T>
    struct TQueueTester {
        TMultiThreadQueue<T> Queue;
        std::atomic<bool> Alive;
        std::mutex Mutex;
        std::unordered_set<T> Result;
        std::vector<std::thread> Workers;

        TQueueTester(int threadCount, int rightBorder, int queueSize)
            : Queue(queueSize)
            , Alive(true)
        {
            for (int i = 0; i != threadCount; ++i) {
                Workers.emplace_back(thread_func<T>, std::ref(Result), std::ref(Alive), std::ref(Queue), std::ref(Mutex));
            }

            for (int i = 0; i != rightBorder; ++i) {
                Queue.Push(T{i});
            }

            while (Queue.Size() != 0) {
                std::mutex m;
                std::unique_lock lock(m);
                auto& cond = Queue.Empty();
                cond.wait(lock);
            }
            Stop();

            ASSERT(Result.size() == static_cast<size_t>(rightBorder), "size mismatch");
        }

        void Stop() {
            Alive = false;
            for (auto& worker : Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        ~TQueueTester() {
            Stop();
        }

    };

    UNIT_TEST(Simple) {
        TQueueTester<int> tester(6, 10000, 1000);
    }
}

int main() {
    GetRegistry()->CreateAndRun();
}
