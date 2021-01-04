#pragma once


template<typename T>
class TSingleton {
public:
    static T& Instance() {
        static T instance;
        return instance;
    };
};