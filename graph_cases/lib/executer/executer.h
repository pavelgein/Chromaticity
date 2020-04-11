#pragma once

#include "queue/queue.h"

#include <memory>
#include <exception>

class ITask {
public:
    virtual void Do() = 0;

    virtual ~ITask() = default;
};

class IExecuter {
public:
    virtual void Add(std::unique_ptr<ITask> &&task) = 0;

    virtual void Stop() = 0;

    virtual size_t Size() = 0;

    virtual ~IExecuter() = default;

};

std::unique_ptr<IExecuter> CreateExecuter(size_t threadCount, size_t maxSize, TMultiThreadQueue<std::exception_ptr>* e);