#include "math_utils/subsets.h"

#include "test_system/test_system.h"

#include <vector>
#include <set>

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
        for (const auto& x: TSubsetGenerator<int>(v)) {
            std::set<int> current;
            for (const auto y : x) {
                current.insert(*y);
            }
            actual.insert(std::move(current));
        }

        ASSERT_EQUAL(expected, actual);
    }
}