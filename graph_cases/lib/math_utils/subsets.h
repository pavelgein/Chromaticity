#pragma once

#include <cstddef>
#include <vector>


template<typename T>
class TSubsetGenerator {
    public:
        class TIterator {
        public:
            TIterator(const std::vector<T>& set, size_t state)
                : Set(set)
                , State(state)
            {
            }

            TIterator(const TIterator& other) = default;

            bool operator==(const TIterator& other) const {
                if (State != other.State) {
                    return false;
                }

                if (Set.size() != other.Set.size()) {
                    return false;
                }

                for (size_t i = 0; i != Set.size(); ++i) {
                    if (Set[i] != other.Set[i]) {
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

            std::vector<const T*> operator*() const {
                std::vector<const T*> result;
                size_t state = State;
                size_t ind = 0;
                while (state) {
                    if (state % 2 == 1) {
                        result.push_back(&Set[ind]);
                    }

                    ++ind;
                    state >>= 1;
                }
                return result;
            }

        private:
            const std::vector<T>& Set;
            size_t State;
        };

        explicit TSubsetGenerator(const std::vector<T>& set)
            : Set(set)
        {
        }

        TIterator begin() const {
            return TIterator{Set, 0};
        }

        TIterator end() const {
            return TIterator{Set, static_cast<size_t>(1) << Set.size()};
        }

    private:
        const std::vector<T>& Set;
};