#include "acyclic_orintations.h"

#include "binomial_coefficients/binomial_coefficients.h"

#include <algorithm>


namespace NMultipartiteGraphs {
    long long TCompleteGraphAcyclicOrientationsCounter::operator()(std::vector<INT> components) {
        std::sort(components.begin(), components.end());

        if (auto iter = Cache.find(components); iter != Cache.end()) {
            return iter->second;
        }

        return Compute(components);
    }

    long long TCompleteGraphAcyclicOrientationsCounter::ComputeUnsafe(const std::vector<INT>& components) {
        if (auto iter = Cache.find(components); iter != Cache.end()) {
            return iter->second;
        }

        if (components.size() == 0) {
            return 1;
        }

        if (components.size() == 1) {
            return 1;
        }

        long long result = 0;
        for (size_t i = 0; i != components.size(); ++i) {
            result += ComputeUnsafeForComponent(components, i);
        }

        Cache[components] = result;
        return result;
    }

    long long TCompleteGraphAcyclicOrientationsCounter::Compute(const std::vector<INT>& components) {
        std::lock_guard<std::mutex> lock(M);
        return ComputeUnsafe(components);
    }

    long long TCompleteGraphAcyclicOrientationsCounter::ComputeUnsafeForComponent(const std::vector<INT>& components, size_t component) {
        auto componentSize = components[component];
        long long result = 0;
        std::vector<INT> newComponents = components;
        for (INT j = 1; j < componentSize; ++j) {
            newComponents[component] = componentSize - j;

            long long localResult = 0;
            for (size_t newComponent = 0; newComponent != components.size(); ++newComponent) {
                if (newComponent != component) {
                    localResult += ComputeUnsafeForComponent(newComponents, newComponent);
                }
            }

            result += BinomialCoefficient(componentSize, j) * localResult;
        }

        for (size_t componentIndex = component; componentIndex + 1 != components.size(); ++componentIndex) {
            newComponents[componentIndex] = newComponents[componentIndex + 1];
        }

        newComponents.pop_back();
        result += ComputeUnsafe(newComponents);
        return result;
    }
}
