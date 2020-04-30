#include "test_system/test_system.h"

#include <vector>

template<typename T>
void AssertVectors(const std::vector<T>& left, const std::vector<T>& right) {
    ASSERT(left.size() == right.size(), "sizes mismatch");
    for (size_t i = 0; i != left.size(); ++i) {
        ASSERT(left[i] == right[i], "failed at " << i);
    }
}