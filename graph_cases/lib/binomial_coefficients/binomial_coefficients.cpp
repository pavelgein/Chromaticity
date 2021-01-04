#include "binomial_coefficients.h"

#include "singleton/singleton.h"

#include <vector>
#include <deque>
#include <mutex>

class TBinomialCoefficientCalculator {
public:
    TBinomialCoefficientCalculator() = default;

    long long operator()(unsigned int n, unsigned int k) {
        if ((k == 0) || (k == n)) {
            return 1;
        }

        if ((k == 1) || (k + 1 == n)) {
            return n;
        }

        if ((k == 2) || (k + 2 == n)) {
            return n * (n - 1) / 2;
        }

        if (n < Cache.size()) {
            return Cache[n][k];
        }

        return Compute(n, k);
    }

private:
    long long Compute(unsigned int n, unsigned int k) {
        std::lock_guard<std::mutex> lock(M);
        for (unsigned int m = Cache.size(); m <= n; m++) {
            Cache.push_back(ComputeRow(m));
        }

        return Cache[n][k];
    }

    std::vector<long long> ComputeRow(unsigned int rowNumber) {
        std::vector<long long> result;
        result.reserve(rowNumber + 1);
        result.push_back(1);
        const auto& prevRow = Cache[rowNumber - 1];
        for (unsigned int i = 1; i < rowNumber; ++i) {
            result.push_back(prevRow[i] + prevRow[i - 1]);
        }

        result.push_back(1);

        return result;
    }

    std::deque<std::vector<long long>> Cache = {{1}};
    std::mutex M;
};

long long BinomialCoefficient(unsigned int n, unsigned int k) {
    return TSingleton<TBinomialCoefficientCalculator>::Instance()(n, k);
}
