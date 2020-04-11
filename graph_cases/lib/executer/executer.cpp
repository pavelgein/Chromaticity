#include "executer.h"
#include "multithread_executer.h"
#include "singlethread_exectuer.h"

#include "queue/queue.h"


std::unique_ptr<IExecuter> CreateExecuter(size_t threadCount, size_t maxSize, TMultiThreadQueue<std::exception_ptr>* e) {
    if (threadCount == 1) {
       return std::make_unique<TSingleThreadExecuter>();
    }

    return std::make_unique<TMultiThreadExecuter>(threadCount, maxSize, e);
}