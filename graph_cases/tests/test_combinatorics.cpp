#include "utils.h"

#include "math_utils/combinatorics.h"
#include "test_system/test_system.h"

#include <vector>

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
            ASSERT(pair == answers[i], "iteration " << i << " failed");
            ++i;
        }

        ASSERT(i == answers.size(), "wrong number of iterations");
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

        ASSERT(iter == answers.size(), "wrong number of iterations");
    }
}