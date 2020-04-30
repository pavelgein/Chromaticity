#pragma once

template<typename TIterator>
typename TIterator::value_type SumRange(TIterator begin, TIterator end) {
    typename TIterator::value_type ans{};
    for (auto iter = begin; iter != end; ++iter) {
        ans += *iter;
    }

    return ans;
}