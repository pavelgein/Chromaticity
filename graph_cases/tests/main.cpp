#include "test_system/test_system.h"


int main() {
    TTestRunStat stat = GetRegistry()->CreateAndRun();
    WriteStat(stat, std::cout);
    bool success = (stat.Failed == 0) && (stat.Crashed == 0);
    return success ? 0 : 1;
}
