#include "autoindexer/autoindexer.h"

#include "test_system/test_system.h"

#include <string>

UNIT_TEST_SUITE(AutoIndexer) {
    UNIT_TEST(Unbound) {
        TAutoIndexer<std::string> indexer;
        {
            auto ind = indexer.GetIndex("aa");
            ASSERT_EQUAL(0, ind);
        }
        {
            auto ind = indexer.GetIndex("b");
            ASSERT_EQUAL(1, ind);
        }
        {
            auto ind = indexer.GetIndex("aa");
            ASSERT_EQUAL(0, ind);
        }
    }

    UNIT_TEST(Bound) {
        TAutoIndexer<std::string> indexer{2};
        ASSERT_EQUAL(0, indexer.GetIndex("a"));
        ASSERT_EQUAL(1, indexer.GetIndex("b"));

        bool caught = false;
        try {
            indexer.GetIndex("c");
        } catch(const TAutoIndexer<std::string>::TFullException&) {
            caught = true;
        }

        ASSERT(caught, "exception shoulb be thrown");
    }
}