#pragma once

#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <functional>

struct TTestRunStat {
    int Success = 0;
    int Failed = 0;
    int Crashed = 0;
};

void WriteStat(const TTestRunStat& stat, std::ostream& outp);

class ITest {
public:
    ITest() = default;

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
    using TCreator = std::function<ITest*()>;

    TTestRegistry() = default;

    void Run(ITest* test, TTestRunStat& stat);

    TTestRegistry& Add(ITest* test);

    void AddCreator(TCreator creator) {
        Creators.push_back(creator);
    }

    TTestRunStat CreateAndRun();

private:
    std::vector<TCreator> Creators;
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
#define ASSERT(__cond, __msg) do {                                                                    \
    if (!(__cond)) {                                                                                  \
        std::stringstream ss;                                                                         \
        ss << __FILE__ << ":" << __LINE__ << " Condition " << #__cond << " failed: " << __msg;        \
        FAIL(ss.str());                                                                               \
    }                                                                                                 \
} while(0)

#define ASSERT_EQUAL_WITH_MESSAGE(__a, __b,  __msg) ASSERT((__a) == (__b), __msg)
#define ASSERT_EQUAL(__a, __b) ASSERT_EQUAL_WITH_MESSAGE(__a, __b, "")



#define UNIT_TEST(name) class TTest##name : public ITest {                        \
public:                                                                           \
    std::string Name;                                                             \
    TTest##name()                                                                 \
        : Name(#name)                                                             \
    {                                                                             \
    }                                                                             \
    void Run() const override;                                                    \
    std::string GetLocalName() const override { return Name;}                     \
    static TTest##name* Create() {                                                \
        return new TTest##name();                                                 \
    }                                                                             \
                                                                                  \
    struct TRegistrator {                                                         \
        TRegistrator() {                                                          \
            GetRegistry()->AddCreator(TTest##name::Create);                       \
        }                                                                         \
    };                                                                            \
                                                                                  \
    static TRegistrator Registrator;                                              \
};                                                                                \
TTest##name::TRegistrator TTest##name::Registrator = TTest##name::TRegistrator(); \
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


