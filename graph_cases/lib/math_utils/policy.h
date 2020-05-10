#pragma once


namespace NPolicy {
    template<typename TObjectType>
    struct TPointerPolicy {
        using value_type = const TObjectType*;
        static value_type CreateObject(const TObjectType* object) {
            return object;
        }
    };

    template<typename TObjectType>
    struct TCopyPolicy {
        using value_type = TObjectType;
        static value_type CreateObject(const TObjectType* object) {
            return *object;
        }
    };
};