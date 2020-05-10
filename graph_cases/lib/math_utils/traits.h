#pragma once

#include <utility>

template<typename TIterator>
struct TIteratorDereferencer {
    using TValueType = decltype(*std::declval<TIterator>());
    using TUnreferenced = typename std::remove_reference<TValueType>::type;
    using TRawType = typename std::remove_const<TUnreferenced>::type;
};