#pragma once

#include "sum.h"
#include "traits.h"

#include <cstddef>
#include <iterator>

template<typename TIterator>
typename TIteratorDereferencer<TIterator>::TRawType Sigma(size_t n, TIterator begin, TIterator end) {
    if (n == 1) {
        return SumRange(begin, end);
    }

    size_t startDistance = std::distance(begin, end);
    typename TIteratorDereferencer<TIterator>::TRawType ans{};
    for (size_t i = 0; i + n - 1 != startDistance; ++i) {
        auto value = *begin;
        ++begin;
        ans += value * Sigma(n - 1, begin, end);
    }

    return ans;
}

template<typename TContainer>
typename TContainer::value_type Sigma(size_t n, const TContainer& container) {
    return Sigma(n, std::begin(container), std::end(container));
}