#pragma once

template<class TContainer>
typename TContainer::value_type sigma2(const TContainer& container) {
    using TValue = typename TContainer::value_type;
    TValue ans = TValue();
    for (auto iter1 = container.begin(); iter1 != container.end(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.end(); ++iter2) {
            if (iter1 == iter2) {
                continue;
            }
            ans += (*iter1) * (*iter2);
        }
    }
    return ans;
}

template<class TContainer>
typename TContainer::value_type sigma3(const TContainer& container) {
    typename TContainer::value_type ans{};
    for (auto iter1 = container.begin(); iter1 != container.end(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.end(); ++iter2) {
            if (iter1 == iter2) {
                continue;
            }
            for (auto iter3 = iter2; iter3 != container.end(); ++iter3) {
                if (iter3 == iter1 || iter3 == iter2) {
                    continue;
                }
                ans += (*iter1) * (*iter2) * (*iter3);
            }
        }
    }
    return ans;
}

template<typename TInt>
TInt c_n_2(TInt n) {
    return n * (n - 1) / 2;
}


template<typename TIterator>
typename TIterator::value_type SumRange(TIterator begin, TIterator end) {
    typename TIterator::value_type ans{};
    for (auto iter = begin; iter != end; ++iter) {
        ans += *iter;
    }

    return ans;
}

template<typename TIterator>
typename TIterator::value_type Sigma(size_t n, TIterator begin, TIterator end) {
    if (n == 1) {
        return SumRange(begin, end);
    }

    size_t startDistance = std::distance(begin, end);
    typename TIterator::value_type ans{};
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

class TPairGenerator {
public:
    using TPair = std::pair<size_t, size_t>;

    TPairGenerator(size_t n)
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

    TIterator begin() {
        return TIterator(N, K);
    }

    const TIterator& end() const {
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