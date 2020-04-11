#include "executer.h"

#include "queue/queue.h"

#include <variant>
#include <vector>

struct TStopMessage {};

class TMultiThreadExecuter : public IExecuter {
public:
    explicit TMultiThreadExecuter(size_t theadCount, size_t queueSize, TMultiThreadQueue<std::exception_ptr>* exceptions);

    void Add(std::unique_ptr<ITask>&& task) override;

    size_t Size() override;

    void Stop() override;

    ~TMultiThreadExecuter() override;

private:
    void StopAll();
    void WaitAll();

    using TItem = std::variant<TStopMessage, std::unique_ptr<ITask>>;
    TMultiThreadQueue<TItem> Queue;
    std::vector<std::thread> Threads;
    TMultiThreadQueue<std::exception_ptr>* Exceptions;
};

TMultiThreadExecuter::TMultiThreadExecuter(size_t threadCount, size_t queueSize, TMultiThreadQueue<std::exception_ptr>* exceptions)
    : Queue{queueSize}
    , Threads{}
    , Exceptions(exceptions)
{
    for (size_t i = 0; i != threadCount; ++i) {
        Threads.emplace_back([this](){
            while (true) {
                auto item = Queue.Pop();
                if (item.index() == 0) {
                   break;
                }

                try {
                    std::get<std::unique_ptr<ITask>>(item)->Do();
                } catch (...) {
                    if (Exceptions) {
                        Exceptions->Push(std::current_exception());
                    }
                }
            }
        });
    }
}

void TMultiThreadExecuter::Add(std::unique_ptr<ITask>&& task) {
   Queue.Push(std::move(task));
}

void TMultiThreadExecuter::Stop() {
    StopAll();
    WaitAll();
}

size_t TMultiThreadExecuter::Size() {
    return Queue.Size();
}

void TMultiThreadExecuter::StopAll() {
    for (size_t i = 0; i != Threads.size(); ++i) {
        Queue.Push(TItem(TStopMessage()));
    }
}

void TMultiThreadExecuter::WaitAll() {
    for (auto& thread : Threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

TMultiThreadExecuter::~TMultiThreadExecuter() {
    Stop();
}

class TSingleThreadExecuter : public IExecuter {
public:
    void Add(std::unique_ptr<ITask>&& task) override;

    void Stop() override;

    size_t Size() override;

    virtual ~TSingleThreadExecuter() = default;
};

void TSingleThreadExecuter::Add(std::unique_ptr<ITask>&& task) {
    task->Do();
}

void TSingleThreadExecuter::Stop() {

}

size_t TSingleThreadExecuter::Size() {
    return 0;
}

std::unique_ptr<IExecuter> CreateExecuter(size_t threadCount, size_t maxSize, TMultiThreadQueue<std::exception_ptr>* e) {
    if (threadCount == 1) {
       return std::make_unique<TSingleThreadExecuter>();
    }

    return std::make_unique<TMultiThreadExecuter>(threadCount, maxSize, e);
}