#include "test_system.h"

#include <iostream>
#include <vector>

TTestRegistry& TTestRegistry::Add(ITest* test) {
    Tests.push_back(test);
    return *this;
}

void TTestRegistry::RunAll() {
    TTestRunStat stat;
    for (auto test: Tests) {
        Run(test, stat);
    }

    std::cerr << "**************************" << std::endl;
    std::cerr << " * Success: " << stat.Success << std::endl;
    std::cerr << " * Failed: " << stat.Failed << std::endl;
    std::cerr << " * Crashed: " << stat.Crashed << std::endl;
}

void TTestRegistry::Run(ITest* test, TTestRunStat& stat) {
    try {
        test->Run();
        std::cerr << test->GetName() << " OK\n";
        stat.Success++;
    } catch (const TTestFail& fail) {
        std::cerr << test->GetName() << " Failed: " << fail.what() << std::endl;
        stat.Failed++;
    } catch (...) {
        std::cerr << test->GetName() << " Crashed " << std::endl;
        stat.Crashed++;
    }
}

ITest::ITest() {
    GetRegistry()->Add(this);
}

TTestRegistry* GetRegistry() {
    static TTestRegistry* registry;
    if (!registry) {
        registry = new TTestRegistry();
    }

    return registry;
}

