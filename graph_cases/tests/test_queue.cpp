#include "test_system/test_system.h"

#include "queue/queue.h"

#include <atomic>
#include <string>
#include <unordered_set>

struct TSimpleStruct {
    std::string S;
    int I;

    TSimpleStruct(int i)
        : S('a', i)
        , I(i)
    {
    }

    TSimpleStruct(const TSimpleStruct& other)
        : S(other.S)
        , I(other.I)
    {
    }

    TSimpleStruct()
        : TSimpleStruct(0)
    {
    }

    TSimpleStruct& operator=(TSimpleStruct&& other) {
       S = std::move(other.S);
       I = other.I;
       return *this;
    }

    TSimpleStruct(TSimpleStruct&& other) = default;

    bool operator==(const TSimpleStruct& other) const {
        return (S == other.S) && (I == other.I);
    }
};

namespace std {
template<>
struct hash<TSimpleStruct> {
    size_t operator()(const TSimpleStruct& s) const {
        return std::hash<std::string>()(s.S);
    }
};
}


UNIT_TEST_SUITE(Queue) {
    template<typename T>
    void thread_func(std::unordered_set<T>& set, std::atomic<bool>& alive, TMultiThreadQueue<T>& queue, std::mutex& m) {
        while (alive) {
            T result;
            if (queue.Pop(std::chrono::seconds(1), result)) {
                std::lock_guard<std::mutex> lock{m};
                set.insert(result);
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    template<typename T>
    struct TQueueTester {
        TMultiThreadQueue<T> Queue;
        std::atomic<bool> Alive;
        std::mutex Mutex;
        std::unordered_set<T> Result;
        std::vector<std::thread> Workers;

        TQueueTester(int threadCount, int rightBorder, int queueSize)
            : Queue(queueSize)
            , Alive(true)
        {
            for (int i = 0; i != threadCount; ++i) {
                Workers.emplace_back(thread_func<T>, std::ref(Result), std::ref(Alive), std::ref(Queue), std::ref(Mutex));
            }

            for (int i = 0; i != rightBorder; ++i) {
                Queue.Push(T{i});
            }

            Queue.WaitEmpty();
            Stop();

            ASSERT(Result.size() == static_cast<size_t>(rightBorder), "size mismatch");
        }

        void Stop() {
            Alive = false;
            for (auto& worker : Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        ~TQueueTester() {
            Stop();
        }

    };

    UNIT_TEST(Simple) {
        TQueueTester<int> tester(6, 10000, 1000);
    }

    UNIT_TEST(SimpleStruct) {
        TQueueTester<TSimpleStruct> tester(6, 10000, 1000);
    }
}
