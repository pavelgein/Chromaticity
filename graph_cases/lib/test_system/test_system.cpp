#include "test_system.h"

#include <vector>

void WriteStat(const TTestRunStat& stat, std::ostream& outp) {
    outp << "**************************" << std::endl;
    outp << " * Success: " << stat.Success << std::endl;
    outp << " * Failed: " << stat.Failed << std::endl;
    outp << " * Crashed: " << stat.Crashed << std::endl;
    outp << " * Skipped: " << stat.Skipped << std::endl;
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

TTestRunStat TTestRegistry::CreateAndRun(ITestFilter* filter) {
    TTestRunStat stat;
    for (const auto& creator: Creators) {
        ITest* test = creator();
        if (!filter || filter->Check(test)) {
            Run(test, stat);
        } else {
            stat.Skipped += 1;
        }
        delete test;
    }

    return stat;
}

TTestRegistry* GetRegistry() {
    static TTestRegistry* registry;
    if (!registry) {
        registry = new TTestRegistry();
    }

    return registry;
}

