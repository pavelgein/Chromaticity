#include "factorial.h"

#include "singleton/singleton.h"

#include <deque>
#include <mutex>


class TFactorialCalcer {
public:
    long long operator()(unsigned int n) {
        if (n <= 1) {
            return 1;
        }

        if (n < Cache.size()) {
            return Cache[n];
        }

        return Calculate(n);
    }

private:
    long long Calculate(unsigned int n) {
        std::lock_guard<std::mutex> lock(M);
        auto fact = Cache.back();
        for (size_t i = Cache.size(); i <= n; ++i) {
            fact *= i;
            Cache.push_back(fact);
        }

        return Cache.back();
    }

    std::deque<long long> Cache = {1};
    std::mutex M;
};


long long Factorial(unsigned int n) {
    return TSingleton<TFactorialCalcer>::Instance()(n);
}
