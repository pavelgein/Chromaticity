#include "math_utils/subsets.h"

#include "test_system/test_system.h"

#include <vector>
#include <set>
#include <unordered_set>

UNIT_TEST_SUITE(Subsets) {
    UNIT_TEST(Vector) {
        std::vector<int> v = {1, 2};
        std::set<std::set<int>> expected = {
            {},
            {1},
            {2},
            {1, 2},
        };

        std::set<std::set<int>> actual;
        for (const auto& x: TSubsetGenerator(v.begin(), v.end())) {
            std::set<int> current;
            for (const auto y : x) {
                current.insert(*y);
            }
            actual.insert(std::move(current));
        }

        ASSERT_EQUAL(expected, actual);
    }

    UNIT_TEST(Set) {
        std::unordered_set<int> v = {1, 2};
        std::set<std::set<int>> expected = {
            {},
            {1},
            {2},
            {1, 2},
        };

        std::set<std::set<int>> actual;
        for (const auto& x: TSubsetGenerator(v.begin(), v.end())) {
            std::set<int> current;
            for (const auto y : x) {
                current.insert(*y);
            }
            actual.insert(std::move(current));
        }

        ASSERT_EQUAL(expected, actual);
    }
}