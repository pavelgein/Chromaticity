#include "test_system/test_system.h"

#include "math_utils/sigma.h"

#include <vector>

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