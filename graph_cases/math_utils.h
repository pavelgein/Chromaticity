#pragma once

template<typename T, class TContainer>
T sigma2(const TContainer& container) {
    T ans = 0;
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

template<typename T, class TContainer>
T sigma3(const TContainer& container) {
    T ans = 0;
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


template<typename T, class TContainer>
T sum_collection(const TContainer& container, T init_value) {
    T sum = init_value;
    for (const auto& x: container) {
        sum += x;
    }
    return sum;
}
