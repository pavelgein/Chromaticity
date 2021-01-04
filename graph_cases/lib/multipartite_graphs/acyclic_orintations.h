#pragma once

#include "local_types.h"

#include <vector>
#include <unordered_map>
#include <mutex>


namespace NMultipartiteGraphs {
    class TCompleteGraphAcyclicOrientationsCounter {
    public:
        long long operator()(std::vector<INT> components);

    private:
        class TVectorHasher {
        public:
           size_t operator()(const std::vector<INT>& v) const {
               size_t result = 0;
               std::hash<INT> elementHasher{};
               for (const auto x : v) {
                   result ^= elementHasher(x);
               }

               return result;
           }
        };

        long long Compute(const std::vector<INT>& components);

        long long ComputeUnsafe(const std::vector<INT>& components);

        long long ComputeUnsafeForComponent(const std::vector<INT>& components, size_t component);

        std::unordered_map<std::vector<INT>, long long, TVectorHasher> Cache;
        std::mutex M;
    };
}