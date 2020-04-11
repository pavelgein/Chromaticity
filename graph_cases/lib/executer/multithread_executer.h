#include "executer.h"

#include <variant>
#include <thread>
#include <vector>
#include <exception>

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

