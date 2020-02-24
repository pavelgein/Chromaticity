#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <chrono>

template<typename TType>
class TMultiThreadQueue {
public:
    TMultiThreadQueue(size_t maxSize = 0)
            : MaxSize(maxSize)
    {
    }

    TType Pop() {
        std::unique_lock<std::mutex> lock(Mutex);
        while (Storage.empty()) {
            NonEmpty.wait(lock);
        }

        TType front;
        DoPop(front);
        return front;
    }

    bool Pop(std::chrono::seconds timeout, TType& result) {
        std::unique_lock<std::mutex> lock(Mutex);
        while (Storage.empty()) {
            if (NonEmpty.wait_for(lock, timeout) == std::cv_status::timeout) {
                return false;
            }
        }

        DoPop(result);
        return true;
    }

    void Push(TType item) {
        std::unique_lock<std::mutex> lock(Mutex);
        if (MaxSize != 0) {
            while (Storage.size() >= MaxSize) {
                Pushable.wait(lock);
            }
        } else {
            lock.lock();
        }

        Storage.push_back(std::move(item));
        NonEmpty.notify_one();
    }

    std::condition_variable& Empty() {
        return Empty_;
    }

    size_t Size() const {
        return Storage.size();
    }

private:
    void DoPop(TType& result){
        result = std::move(Storage.front());
        Storage.pop_front();
        if (Storage.empty()) {
            Empty_.notify_all();
        }

        if (Storage.size() < MaxSize) {
            Pushable.notify_one();
        }
    }

    size_t MaxSize;
    std::deque<TType> Storage;
    std::mutex Mutex;
    std::condition_variable NonEmpty;
    std::condition_variable Empty_;
    std::condition_variable Pushable;
};
