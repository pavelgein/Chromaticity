#include "utils.h"

#include "test_system/test_system.h"

#include "multipartite_graphs/multipartite_graphs.h"

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
        ASSERT(9 == completeGraph.PtInvariant(), "pt invariant mismatched");

        ASSERT(graph.I2Invariant() == 26, "i2 invariant mismatched");
        ASSERT(graph.I3Invariant() == 24, "i3 invariant mismatched");
        ASSERT(graph.I4Invariant() == 26, "i4 invariant mismatched");
        ASSERT(graph.PtInvariant() == 10, "pt invariant mismatched");
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
        ASSERT(graph.PtInvariant() == 12, "pt invariant mismatched");
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
        ASSERT(graph.PtInvariant() == 11, "pt invariant mismatched");
    }

    UNIT_TEST(TextXi2_2) {
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 1), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 1)),
            TEdge(TVertex(0, 1), TVertex(1, 2)),
            TEdge(TVertex(0, 2), TVertex(1, 0)),
            TEdge(TVertex(0, 2), TVertex(1, 1)),
            TEdge(TVertex(0, 2), TVertex(1, 2)),
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(1, 3), TVertex(2, 3)),
        };

        auto completeGraph = TCompleteGraph({4, 4, 3});
        TDenseGraph graph(completeGraph, edges);

        ASSERT(graph.I2Invariant() == 32, "i2 invariant mismatched");
        ASSERT(graph.I3Invariant() == 23, "i3 invariatn mismatched");
    }

    UNIT_TEST(TextXi2_3) {
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 0)),

            TEdge(TVertex(0, 2), TVertex(1, 1)),
            TEdge(TVertex(0, 3), TVertex(1, 1)),

            TEdge(TVertex(1, 0), TVertex(2, 0)),
            TEdge(TVertex(1, 0), TVertex(2, 1)),

            TEdge(TVertex(1, 1), TVertex(2, 1)),
            TEdge(TVertex(1, 1), TVertex(2, 2)),
        };

        auto completeGraph = TCompleteGraph({4, 4, 3});
        TDenseGraph graph(completeGraph, edges);

        ASSERT(graph.I2Invariant() == 32, "i2 invariant mismatched");
        ASSERT(graph.I3Invariant() == 28, "i3 invariatn mismatched");

    }
}

UNIT_TEST_SUITE(TestSwapVertices) {
    UNIT_TEST(TestSwap) {
        using namespace NMultipartiteGraphs;
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 0)),

            TEdge(TVertex(0, 2), TVertex(1, 1)),
            TEdge(TVertex(0, 3), TVertex(1, 1)),

            TEdge(TVertex(1, 0), TVertex(2, 0)),
            TEdge(TVertex(1, 0), TVertex(2, 1)),

            TEdge(TVertex(1, 1), TVertex(2, 1)),
            TEdge(TVertex(1, 1), TVertex(2, 2)),
        };

        auto completeGraph = TCompleteGraph({4, 4, 3});
        TDenseGraph graph(completeGraph, edges);

        {
            TDenseGraph newGraph = graph.SwapVertices(0, 0, 2);

            TEdgeSet newEdges = {
                TEdge(TVertex(0, 2), TVertex(1, 0)),
                TEdge(TVertex(0, 1), TVertex(1, 0)),

                TEdge(TVertex(0, 0), TVertex(1, 1)),
                TEdge(TVertex(0, 3), TVertex(1, 1)),

                TEdge(TVertex(1, 0), TVertex(2, 0)),
                TEdge(TVertex(1, 0), TVertex(2, 1)),

                TEdge(TVertex(1, 1), TVertex(2, 1)),
                TEdge(TVertex(1, 1), TVertex(2, 2)),
            };
            ASSERT_EQUAL(newGraph.DeletedEdges(), newEdges);
        }
    }

    UNIT_TEST(TestSwapInplace) {
        using namespace NMultipartiteGraphs;
        using namespace NMultipartiteGraphs;

        TEdgeSet edges = {
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(0, 1), TVertex(1, 0)),

            TEdge(TVertex(0, 2), TVertex(1, 1)),
            TEdge(TVertex(0, 3), TVertex(1, 1)),

            TEdge(TVertex(1, 0), TVertex(2, 0)),
            TEdge(TVertex(1, 0), TVertex(2, 1)),

            TEdge(TVertex(1, 1), TVertex(2, 1)),
            TEdge(TVertex(1, 1), TVertex(2, 2)),
        };

        auto completeGraph = TCompleteGraph({4, 4, 3});
        TDenseGraph graph(completeGraph, edges);

        {
            TEdgeSet newEdges = {
                TEdge(TVertex(0, 2), TVertex(1, 0)),
                TEdge(TVertex(0, 1), TVertex(1, 0)),

                TEdge(TVertex(0, 0), TVertex(1, 1)),
                TEdge(TVertex(0, 3), TVertex(1, 1)),

                TEdge(TVertex(1, 0), TVertex(2, 0)),
                TEdge(TVertex(1, 0), TVertex(2, 1)),

                TEdge(TVertex(1, 1), TVertex(2, 1)),
                TEdge(TVertex(1, 1), TVertex(2, 2)),
            };

            graph.SwapVerticesInplace(0, 0, 2);
            ASSERT_EQUAL(graph.DeletedEdges(), newEdges);
        }
    }
}

UNIT_TEST_SUITE(TestContractEdge) {
    UNIT_TEST(Simple) {
        using namespace NMultipartiteGraphs;
        TCompleteGraph graph({3, 3, 1});
        TEdgeSet edges = {
            TEdge(TVertex(0, 0), TVertex(1, 0)),
            TEdge(TVertex(0, 0), TVertex(2, 0)),
        };

        TDenseGraph denseGraph(graph, edges);

        {
            std::vector<TEdge> contractedEdges = {
                {TVertex(0, 0), TVertex(1, 0)},
                {TVertex(1, 0), TVertex(0, 0)}
            };
            for (const auto &edge : contractedEdges) {
                auto[newDenseGraph, newGraph] = denseGraph.ContractEdge(edge);
                ASSERT_EQUAL(newGraph->ComponentsNumber(), 4);
                ASSERT_EQUAL(newGraph->ComponentSize(0), 2);
                ASSERT_EQUAL(newGraph->ComponentSize(1), 2);
                ASSERT_EQUAL(newGraph->ComponentSize(2), 1);
                ASSERT_EQUAL(newGraph->ComponentSize(3), 1);

                TEdgeSet newEdges = {};
                ASSERT_EQUAL(newDenseGraph.DeletedEdges(), newEdges);
            }
        }

        {
            std::vector<TEdge> contractedEdges = {
                {TVertex(0, 0), TVertex(2, 0)},
                {TVertex(2, 0), TVertex(0, 0)}
            };
            for (const auto& edge : contractedEdges) {
                auto [newDenseGraph, newGraph] = denseGraph.ContractEdge(edge);
                ASSERT_EQUAL(newGraph->ComponentsNumber(), 3);
                ASSERT_EQUAL(newGraph->ComponentSize(0), 2);
                ASSERT_EQUAL(newGraph->ComponentSize(1), 3);
                ASSERT_EQUAL(newGraph->ComponentSize(2), 1);

                TEdgeSet newEdges = {};
                ASSERT_EQUAL(newDenseGraph.DeletedEdges(), newEdges);
            }
        }

        {
            auto [newDenseGraph, newGraph] = denseGraph.ContractEdge({TVertex(0, 1), TVertex(1, 1)});
            ASSERT_EQUAL(newGraph->ComponentsNumber(), 4);
            ASSERT_EQUAL(newGraph->ComponentSize(0), 2);
            ASSERT_EQUAL(newGraph->ComponentSize(1), 2);
            ASSERT_EQUAL(newGraph->ComponentSize(2), 1);
            ASSERT_EQUAL(newGraph->ComponentSize(3), 1);

            ASSERT_EQUAL(newDenseGraph.DeletedEdges(), denseGraph.DeletedEdges());
        }
    }

    UNIT_TEST(Simple2) {
        using namespace NMultipartiteGraphs;
        TCompleteGraph graph({2, 2});
        TEdgeSet edges = {
            {TVertex(0, 0), TVertex(1, 0)},
            {TVertex(0, 0), TVertex(1, 1)},
        };

        auto edge = TEdge({0, 0}, {1,0});
        TDenseGraph denseGraph(graph, edges);
        auto [contracted, base] = denseGraph.ContractEdge(edge);
        ASSERT_EQUAL(*base, TCompleteGraph({1, 1, 1}));
        ASSERT_EQUAL(contracted.DeletedEdges(), TEdgeSet({{TVertex(1, 0), TVertex(2, 0)}}));
    }
}

UNIT_TEST_SUITE(TestAcyclicOrientations) {
    UNIT_TEST(TestOnePart) {
        NMultipartiteGraphs::TCompleteGraph graph({5});
        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 1);
    }

    UNIT_TEST(TestTwoParts) {
        NMultipartiteGraphs::TCompleteGraph graph({1, 1});
        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 2);
    }

    UNIT_TEST(TestTwoParts2) {
        NMultipartiteGraphs::TCompleteGraph graph({2, 1});
        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 4);
    }

    UNIT_TEST(TestTwoParts3) {
        NMultipartiteGraphs::TCompleteGraph graph({3, 4});
        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 1066);
    }

    UNIT_TEST(TestThreeParts) {
        NMultipartiteGraphs::TCompleteGraph graph({7, 2, 2});
        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 1682766);
    }

    UNIT_TEST(TestDense) {
        using namespace NMultipartiteGraphs;
        TCompleteGraph graph({2, 1});
        TEdgeSet edges = {
            {TVertex(0, 0), TVertex(1, 0)},
        };
        NMultipartiteGraphs::TDenseGraph denseGraph(graph, edges);
        denseGraph.CountAcyclicOrientations();
        ASSERT_EQUAL(denseGraph.CountAcyclicOrientations(), 2);
    }

    UNIT_TEST(TestDense2) {
        using namespace NMultipartiteGraphs;
        TCompleteGraph graph({2, 2});
        TEdgeSet edges = {
            {TVertex(0, 0), TVertex(1, 0)},
            {TVertex(0, 0), TVertex(1, 1)},
        };

        ASSERT_EQUAL(graph.CountAcyclicOrientations(), 14);
        NMultipartiteGraphs::TDenseGraph denseGraph(graph, edges);
        ASSERT_EQUAL(denseGraph.CountAcyclicOrientations(), 4);
    }
}