#pragma once

#include "policy.h"
#include "traits.h"

#include <cstddef>
#include <vector>


template<typename TIteratorType, typename TPolicy = NPolicy::TPointerPolicy<typename TIteratorDereferencer<TIteratorType>::TUnreferenced>>
class TSubsetGenerator {
    public:
        using element_type = typename TPolicy::value_type;

        class TIterator {
        public:
            using value_type = std::vector<element_type>;

            TIterator(TIteratorType begin, TIteratorType end, size_t state)
                : Begin(begin)
                , End(end)
                , State(state)
            {
            }

            TIterator(const TIterator& other) = default;

            bool operator==(const TIterator& other) const {
                if (State != other.State) {
                    return false;
                }

                auto thisDistance = std::distance(Begin, End);
                auto otherDistance = std::distance(other.Begin, other.End);
                if (thisDistance != otherDistance) {
                    return false;
                }

                for (auto thisIterator = Begin, otherIterator = other.Begin; thisIterator != End && otherIterator != other.End; ++thisIterator, ++otherIterator) {
                    if (*thisIterator != *otherIterator) {
                        return false;
                    }
                }

                return true;
            }

            bool operator!=(const TIterator& other) const {
                return !(*this == other);
            }

            TIterator& operator++() {
                ++State;
                return *this;
            }

            TIterator& operator++(int n) {
                TIterator old = *this;
                State += n;
                return old;
            }

            value_type operator*() const {
                value_type result;
                size_t state = State;
                auto iterator = Begin;
                while (state) {
                    if (state % 2 == 1) {
                        result.push_back(TPolicy::CreateObject(&(*iterator)));
                    }

                    ++iterator;
                    state >>= 1;
                }
                return result;
            }

        private:
            TIteratorType Begin;
            TIteratorType End;
            size_t State;
        };


        explicit TSubsetGenerator(TIteratorType begin, TIteratorType end)
            : Begin(begin)
            , End(end)
        {
        }

        TIterator begin() const {
            return TIterator{Begin, End, 0};
        }

        TIterator end() const {
            return TIterator{Begin, End, static_cast<size_t>(1) << std::distance(Begin, End)};
        }

    private:
        TIteratorType Begin;
        TIteratorType End;
};

template<typename TPolicy, typename TContainer>
auto CreateSubsetGenerator(const TContainer& c) {
    return TSubsetGenerator<decltype(std::begin(c)), TPolicy>(std::begin(c), std::end(c));
}