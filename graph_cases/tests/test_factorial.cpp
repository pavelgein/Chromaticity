#include "test_system/test_system.h"

#include "factorial/factorial.h"

UNIT_TEST_SUITE(TestFactorial) {
    UNIT_TEST(Simple) {
        ASSERT_EQUAL(Factorial(5), 120);
        ASSERT_EQUAL(Factorial(6), 720);
    }
}

