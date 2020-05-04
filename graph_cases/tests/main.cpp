#include "test_system/test_system.h"

#include "optparser/optparser.h"

#include <memory>


struct TOptions {
    std::unique_ptr<ITestFilter> Filter = nullptr;

    static TOptions ParseCommandLind(int argc, const char ** argv) {
        std::string name;
        std::string suite;

        TParser parser;

        parser.AddLongOption('n', "name")
            .Store(&name);

        parser.AddLongOption('s', "suite")
            .Store(&suite);

        parser.Parse(argc, argv);

        std::vector<std::unique_ptr<ITestFilter>> filters;
        if (!name.empty()) {
            filters.push_back(std::make_unique<TTestNameFilter>(std::move(name)));
        }

        if (!suite.empty()) {
            filters.push_back(std::make_unique<TTestSuiteFilter>(std::move(suite)));
        }

        TOptions options;
        if (!filters.empty()) {
            std::unique_ptr<ITestFilter> newFilter = std::make_unique<AndFilter>(std::move(filters));
            options.Filter.swap(newFilter);
        }

        return options;
    }
};


int main(int argc, const char ** argv) {
    auto options = TOptions::ParseCommandLind(argc, argv);
    TTestRunStat stat = GetRegistry()->CreateAndRun(options.Filter.get());
    WriteStat(stat, std::cout);
    bool success = (stat.Failed == 0) && (stat.Crashed == 0);
    return success ? 0 : 1;
}
