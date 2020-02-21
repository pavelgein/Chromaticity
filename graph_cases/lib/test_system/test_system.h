#include <exception>
#include <string>
#include <sstream>
#include <vector>

struct TTestRunStat {
    int Success = 0;
    int Failed = 0;
    int Crashed = 0;
};

class ITest {
public:
    ITest();

    virtual void Run() const = 0;

    virtual ~ITest() = default;

    std::string GetName() {
        auto prefix = GetPrefix();
        if (prefix.empty()) {
            return GetLocalName();
        }

        return prefix + "::" + GetLocalName();
    };

    virtual std::string GetPrefix() {
        return "";
    };

    virtual std::string GetLocalName() const = 0;
};


class TTestRegistry {
public:
    TTestRegistry() = default;

    void RunAll();

    void Run(ITest* test, TTestRunStat& stat);

    TTestRegistry& Add(ITest* test);

private:
    std::vector<ITest*> Tests;
};

TTestRegistry* GetRegistry();


class TTestFail : public std::exception {
private:
    std::string Message;
public:
    TTestFail(std::string message)
        : Message(std::move(message))
    {
    }

    const char * what() const noexcept override {
        return Message.c_str();
    }
};

#define FAIL(msg) throw TTestFail(msg)
#define ASSERT(__cond, __msg) do {\
    if (!(__cond)) {\
        FAIL((std::stringstream() << "Condition " << #__cond << " failed: " << __msg).str()); \
    } \
} while(0)


#define UNIT_TEST(name) class TTest##name : public ITest {\
public: \
    std::string Name; \
    TTest##name() \
        : Name(#name) \
    { \
    } \
    void Run() const override; \
    std::string GetLocalName() const override { return Name;} \
}; \
TTest##name test##name{};    \
    void TTest##name::Run() const

#define UNIT_TEST_SUITE(name) namespace NTestSuite##name {\
    class ITest : public ::ITest { \
    public: \
        ITest() : ::ITest() {}; \
        virtual ~ITest() = default; \
        std::string GetPrefix() override { \
            return #name; \
        } \
    }; \
}\
namespace NTestSuite##name


