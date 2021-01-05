#include "test_system/test_system.h"
#include "executer/executer.h"
#include "singleton/singleton.h"

class TCounter {
public:
    TCounter() {
        std::lock_guard<std::mutex> lock(M);
        ++C_;
    }
    static int C_;

private:
    static std::mutex M;
};

std::mutex TCounter::M;
int TCounter::C_ = 0;


UNIT_TEST_SUITE(TestSingleton) {
    UNIT_TEST(OneThread) {
        class A{};

        auto& s1 = TSingleton<A>::Instance();
        auto& s2 = TSingleton<A>::Instance();

        ASSERT_EQUAL(&s1, &s2);
    }

    UNIT_TEST(Multithread) {
        auto executer = CreateExecuter(10, 1000, nullptr);
        executer->Add(CreateTask([](){
            auto& counter = TSingleton<TCounter>::Instance();
            (void)(counter);
        }));

        executer->Stop();

        ASSERT_EQUAL(TCounter::C_, 1);
    }
}

