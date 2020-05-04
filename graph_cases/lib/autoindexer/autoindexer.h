#pragma once

#include <cstddef>
#include <unordered_map>
#include <exception>

template<typename TKey, typename TStorage=std::unordered_map<TKey, size_t>, typename TInvStorage=std::unordered_map<size_t, const TKey*>>
class TAutoIndexer {
    public:
        class TFullException : public std::exception {
        };

        class TNotFoundException : public std::exception {
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

        const TKey& GetKey(size_t ind) {
            if (auto iter = InvStorage.find(ind); iter != InvStorage.end()) {
                return *(iter->second);
            }

            throw TNotFoundException();
        }

        inline size_t Size() const {
            return Storage.size();
        }

    private:
        size_t DoInsert(const TKey& key) {
            auto result = Storage.size();
            if ((MaxSize != 0) && (result >= MaxSize)) {
                throw TFullException();
            }

            auto pair = Storage.emplace(key, result);
            InvStorage[result] = &(pair.first->first);
            return result;
        }

        size_t MaxSize;
        TStorage Storage;
        TInvStorage InvStorage;
};
