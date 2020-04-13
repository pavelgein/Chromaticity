#include "multithread_executer.h"

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

