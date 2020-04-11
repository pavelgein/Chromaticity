#include <test_system/test_system.h>

#include <executer/executer.h>

#include <mutex>

UNIT_TEST_SUITE(Executer) {
    UNIT_TEST(FunctionTask) {
        std::mutex m;
        size_t n = 0;

        {
            auto executer = CreateExecuter(100, 1000, nullptr);
            for (size_t i = 0; i != 10000; ++i) {
                executer->Add(CreateTask([&m, &n]() mutable {
                    std::lock_guard guard{m};
                    ++n;
                }));
            }
        }

        ASSERT(n == 10000, "bad number of increments");
    }

    class TSimpleException : public std::exception {

    };

    class TSimpleTask : public ITask {
    public:
        explicit TSimpleTask(bool doThrow)
            : DoThrow(doThrow)
        {
        }

        void Do() override {
            if (DoThrow) {
                throw TSimpleException();
            }
        }

    private:
        bool DoThrow;
    };

    UNIT_TEST(Exceptions) {
        TMultiThreadQueue<std::exception_ptr> exceptions(100);
        {
            auto executer = CreateExecuter(5, 100, &exceptions);
            for (size_t i = 0; i != 10; ++i) {
                executer->Add(std::make_unique<TSimpleTask>(i % 2 == 0));
            }
        }

        size_t found = 0;
        while (!exceptions.IsEmpty()) {
            auto ptr = exceptions.Pop();
            try {
                std::rethrow_exception(ptr);
            } catch (TSimpleException&) {
                found += 1;
            } catch (...) {
                FAIL("unexpected exception");
            }
        }

        ASSERT(found == 5, "wrong number of exceptions");
    }
}