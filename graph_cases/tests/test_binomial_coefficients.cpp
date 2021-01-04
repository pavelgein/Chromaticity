#include "test_system/test_system.h"

#include "binomial_coefficients/binomial_coefficients.h"


UNIT_TEST_SUITE(TestBinomialCoefficients) {
    UNIT_TEST(Simple) {
        ASSERT_EQUAL(BinomialCoefficient(5, 0), 1);
        ASSERT_EQUAL(BinomialCoefficient(5, 1), 5);
        ASSERT_EQUAL(BinomialCoefficient(5, 2), 10);
        ASSERT_EQUAL(BinomialCoefficient(5, 3), 10);
        ASSERT_EQUAL(BinomialCoefficient(5, 4), 5);
        ASSERT_EQUAL(BinomialCoefficient(5, 5), 1);
    }
}