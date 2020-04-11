#pragma once

#include "executer.h"

#include <memory>

class TSingleThreadExecuter : public IExecuter {
public:
    void Add(std::unique_ptr<ITask>&& task) override;

    void Stop() override;

    size_t Size() override;

    virtual ~TSingleThreadExecuter() = default;
};


