#include <multithread_writer/writer.h>
#include <executer/executer.h>

#include <test_system/test_system.h>

#include <sstream>
#include <string>
#include <unordered_set>

UNIT_TEST_SUITE(Writer) {
    class TPushTask : public ITask {
    public:
        explicit TPushTask(TWriter& writer, std::string&& s)
            : Writer(writer)
            , S(s)
        {
        }

        void Do() override {
            Writer.Push(std::move(S));
        }

    private:
        TWriter& Writer;
        std::string S;
    };

    UNIT_TEST(Simple) {
        std::stringstream ss{};
        std::unordered_set<size_t> numbers;
        {
            auto writer = TWriter{ss};
            {
                auto executer = CreateExecuter(10, 1000, nullptr);
                for (size_t i = 0; i != 10000; ++i) {
                    executer->Add(std::make_unique<TPushTask>(writer, std::to_string(i) + " "));
                    numbers.insert(i);
                }
            }
        }

        size_t number = 0;
        while (ss >> number) {
            auto iter = numbers.find(number);
            if (iter == numbers.end()) {
                FAIL("does not expect " + std::to_string(number));
            } else {
                numbers.erase(iter);
            }
        }

        ASSERT(numbers.empty(), "all numbers should be read");
    }
}

