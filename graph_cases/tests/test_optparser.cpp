#include "optparser/optparser.h"

#include "test_system/test_system.h"

#include <string>
#include <vector>

UNIT_TEST_SUITE(OptParse) {
    UNIT_TEST(Complex) {
        bool trueFlag;
        bool falseFlag;
        int i;
        std::string s;
        std::string free1;
        std::string free2;

        TParser parser;
        parser.AddLongOption('i', "int")
            .Store(&i);

        parser.AddLongOption("f1").SetFlag(&trueFlag).Default("false");
        parser.AddLongOption("f2").SetFlag(&falseFlag).Default("false");
        parser.AddShortOption('s').Store(&s);

        std::vector<std::string> args = {
            "program", "-i", "2", "--f1", "-s", "source"
        };

        std::vector<const char*> argv;
        for (const auto& arg : args) {
            argv.push_back(arg.data());
        }

        parser.Parse(6, argv.data());

        ASSERT(trueFlag, "trueFlag should be set");
        ASSERT(!falseFlag, "falseFlag should not be set");
        ASSERT(s == "source", "s should be equal to source");
        ASSERT(i == 2, "i should be equal to 2");
    }
}