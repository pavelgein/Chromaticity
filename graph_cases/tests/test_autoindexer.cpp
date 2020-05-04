#include "autoindexer/autoindexer.h"

#include "test_system/test_system.h"

#include <string>

template<typename TException>
class TExceptionCatcher {
    public:
    template<typename TFunc>
    static void Do(TFunc&& f) {
        bool caught = false;
        try {
            f();
        } catch(const TException&) {
            caught = true;
        }

        ASSERT(caught, "exception expected");
    }
};

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

        ASSERT_EQUAL("aa", indexer.GetKey(0));
        ASSERT_EQUAL("b", indexer.GetKey(1));
        TExceptionCatcher<TAutoIndexer<std::string>::TNotFoundException>::Do([&]{
            indexer.GetKey(123);
        });
    }

    UNIT_TEST(Bound) {
        TAutoIndexer<std::string> indexer{2};
        ASSERT_EQUAL(0, indexer.GetIndex("a"));
        ASSERT_EQUAL(1, indexer.GetIndex("b"));

        TExceptionCatcher<TAutoIndexer<std::string>::TFullException>::Do([&]{
            indexer.GetIndex("c");
        });
    }
}