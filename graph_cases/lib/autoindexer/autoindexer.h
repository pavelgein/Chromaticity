#pragma once

#include <cstddef>
#include <unordered_map>
#include <exception>

template<typename TKey, typename TStorage=std::unordered_map<TKey, size_t>>
class TAutoIndexer {
    public:
        class TFullException : public std::exception {
        };

    public:
        explicit TAutoIndexer(size_t maxSize = 0)
            : MaxSize(maxSize)
            , Storage()
        {
        }

        size_t GetIndex(const TKey& key) {
            if (auto iter = Storage.find(key); iter != Storage.end()) {
                return iter->second;
            }
            return DoInsert(key);
        }

    private:
        size_t DoInsert(const TKey& key) {
            auto result = Storage.size();
            if ((MaxSize != 0) && (result >= MaxSize)) {
                throw TFullException();
            }

            Storage.emplace(key, result);
            return result;
        }

        size_t MaxSize;
        TStorage Storage;
};
