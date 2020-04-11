#pragma once

#include "executer/multithread_executer.h"

#include <iostream>

class TWriter {
public:
    explicit TWriter(std::ostream& out)
        : Out(out)
        , Executer(1, 1000, nullptr)
    {
    }

    void Push(std::string&& tokens);

    ~TWriter();

private:
    std::ostream& Out;
    TMultiThreadExecuter Executer;
};



