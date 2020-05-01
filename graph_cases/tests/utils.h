#include "test_system/test_system.h"

#include <vector>

template<typename T>
void AssertVectors(const std::vector<T>& left, const std::vector<T>& right) {
    ASSERT_EQUAL_WITH_MESSAGE(left.size(), right.size(), "sizes mismathed");
    for (size_t i = 0; i != left.size(); ++i) {
        ASSERT_EQUAL_WITH_MESSAGE(left[i], right[i], "failed at " << i);
    }
}