#include "test_system.h"

#include <iostream>
#include <vector>

namespace {
    void WriteStat(const TTestRunStat& stat, std::ostream& outp) {
        outp << "**************************" << std::endl;
        outp << " * Success: " << stat.Success << std::endl;
        outp << " * Failed: " << stat.Failed << std::endl;
        outp << " * Crashed: " << stat.Crashed << std::endl;
    }
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

void TTestRegistry::CreateAndRun() {
    TTestRunStat stat;
    for (const auto& creator: Creators) {
        ITest* test = creator();
        Run(test, stat);
        delete test;
    }

    WriteStat(stat, std::cerr);
}

TTestRegistry* GetRegistry() {
    static TTestRegistry* registry;
    if (!registry) {
        registry = new TTestRegistry();
    }

    return registry;
}

