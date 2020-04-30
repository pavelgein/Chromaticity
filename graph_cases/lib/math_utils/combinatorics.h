#pragma once

#include <cstddef>
#include <utility>
#include <vector>
#include <type_traits>

template<typename TInt>
TInt c_n_2(TInt n) {
    return n * (n - 1) / 2;
}


class TPairGenerator {
public:
    using TPair = std::pair<size_t, size_t>;

    explicit TPairGenerator(size_t n)
        : N(n)
    {
    }

    struct TIterator {
        using value_type = TPair;

        size_t First;
        size_t Second;
        size_t N;

        TIterator(size_t first, size_t second, size_t n)
            : First(first)
            , Second(second)
            , N(n)
        {
        }

        bool operator==(const TIterator& other) const {
            return (First == other.First) && (Second == other.Second) && (N == other.N);
        }

        bool operator!=(const TIterator& other) const {
            return !(*this == other);
        }

        TIterator operator++(int n) {
            auto old = *this;
            for (int i = 0; i != n; ++i) {
                AddOne();
            }

            return old;
        }

        TPair operator*() const {
            return {First, Second};
        }

        TIterator& operator++() {
           AddOne();
           return *this;
        }

        void AddOne() {
            if (Second + 1 == N) {
                ++First;
                Second = First + 1;
            } else {
                ++Second;
            }
        }
    };

    TIterator begin() const {
        return TIterator(0, 1, N);
    }

    TIterator end() const {
        return TIterator(N - 1, N, N);
    }

private:
    size_t N;
};

class TChoiceGenerator {
public:
    class TIterator {
    public:
        using value_type = std::vector<size_t>&;

        TIterator(size_t n, size_t k)
            : N(n)
            , K(k)
            , Combination()
        {
            Combination.reserve(K);
            for (size_t i = 0; i != k; ++i) {
                Combination.push_back(i);
            }

        }

        TIterator(size_t n, size_t k, std::vector<size_t> combination)
            : N(n)
            , K(k)
            , Combination(std::move(combination))
        {
        }

        TIterator(const TIterator& other)
            : N(other.N)
            , K(other.K)
            , Combination(other.Combination)
        {
        }

        TIterator(TIterator&& other) noexcept
            : N(other.N)
            , K(other.K)
            , Combination(std::move(other.Combination))
        {
        }

        TIterator& operator=(const TIterator& other) = default;

        TIterator& operator++() {
            for (size_t i = K; i-- > 0; ) {
                if (Combination[i] < N - K + i) {
                    Combination[i]++;
                    for (size_t j = i + 1; j != K; ++j) {
                        Combination[j] = Combination[j - 1] + 1;
                    }

                    return *this;
                }
            }

            // fake combination
            Combination[0]++;
            return *this;
        }

        const std::vector<size_t>& operator*() const {
            return Combination;
        }

        bool operator==(const TIterator& other) const {
            if ((N != other.N) || (K != other.K)) {
                return false;
            }

            for (size_t i = 0; i != K; ++i) {
                if (Combination[i] != other.Combination[i]) {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const TIterator& other) const {
            return !(*this == other);
        }

    private:
        size_t N;
        size_t K;
        std::vector<size_t> Combination;
    };

public:
    static std::vector<size_t> GenerateMaxCombination(size_t n, size_t k){
        std::vector<size_t> maxCombination;
        maxCombination.reserve(k);
        for (size_t i = 0; i != k; ++i) {
            maxCombination.push_back(n - k + i);
        }

        return maxCombination;
    }

    TChoiceGenerator(size_t n, size_t k)
        : N(n)
        , K(k)
        , End(N, K, GenerateEndCombination(N, K))
    {
    }

    TChoiceGenerator(const TChoiceGenerator& other) = default;

    TChoiceGenerator& operator=(const TChoiceGenerator& other) {
        N = other.N;
        K = other.K;
        End = TIterator{N, K, GenerateEndCombination(N, K)};
        return *this;
    }

    TIterator begin() {
        return TIterator(N, K);
    }

    const TIterator& end() const {
        return End;
    }

    TIterator end() {
        return End;
    }

private:
    static std::vector<size_t> GenerateEndCombination(size_t n, size_t k) {
        auto last = GenerateMaxCombination(n, k);
        ++last[0];
        return last;
    }

private:
    size_t N;
    size_t K;
    TIterator End;
};

namespace std {
    template<>
    struct iterator_traits<TChoiceGenerator> {
        using iterator_category = forward_iterator_tag;
        using value_type = typename TChoiceGenerator::TIterator::value_type;
        using pointer = std::add_pointer<std::remove_reference<TChoiceGenerator::TIterator::value_type>::type>::type;
        using reference = value_type;
    };
}


template<typename TIteratorType>
class TObjectChoiceGenerator {
    using object_type = typename std::iterator_traits<TIteratorType>::value_type;

    public:
        class TIterator {
        public:
            using value_type = std::vector<const object_type*>;

            TIterator(TChoiceGenerator::TIterator indexIterator, const std::vector<const object_type*>& objects)
                : Objects(objects)
                , IndexIterator(indexIterator)
            {
            }

            TIterator(const TIterator& other) = default;

            TIterator(TIterator&& other) = default;

            const value_type operator*() const {
                const auto& indices = *IndexIterator;
                value_type result;
                result.reserve(indices.size());
                for (auto index : indices) {
                    result.push_back(Objects[index]);
                }

                return result;
            }

            TIterator& operator++() {
                ++IndexIterator;
                return *this;
            }

            bool operator==(const TIterator& other) const {
                return (IndexIterator == other.IndexIterator) && ((&Objects) == (&other.Objects));
            }

            bool operator!=(const TIterator& other) const {
                return !(*this == other);
            }

        private:
            const std::vector<const object_type*>& Objects;
            TChoiceGenerator::TIterator IndexIterator;
        };

        TObjectChoiceGenerator(size_t k, TIteratorType begin, TIteratorType end)
            : Objects(CreateObjects(begin, end))
            , Generator(Objects.size(), k)
        {
        }

        TIterator begin()  {
            return TIterator(Generator.begin(), Objects);
        }

        TIterator end() {
            return TIterator(Generator.end(), Objects);
        }

    private:
        static std::vector<const object_type*> CreateObjects(TIteratorType begin, TIteratorType end) {
            std::vector<const object_type*> result;
            while (begin != end) {
                result.push_back(&(*begin));
                ++begin;
            }
            return result;
        }

        std::vector<const object_type*> Objects;
        TChoiceGenerator Generator;
};