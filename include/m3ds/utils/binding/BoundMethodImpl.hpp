#pragma once

#include <algorithm>

#include <m3ds/utils/binding/BoundMethod.hpp>

// TODO: add object_cast for parameter types

namespace M3DS {
    constexpr bool operator==(const MethodTypeIdentifiers& lhs, const MethodTypeIdentifiers& rhs) noexcept {
        if (lhs.returnType != rhs.returnType)
            return false;
        if (lhs.argTypes.size() != rhs.argTypes.size())
            return false;
        if (lhs.argTypes.data() == rhs.argTypes.data())
            return true;
        return std::ranges::equal(lhs.argTypes, rhs.argTypes);
    }

    template <bool IsConst>
    template <std::size_t N>
    constexpr GenericMethod<IsConst>::GenericMethod(const char(& name)[N]) noexcept : mName(name) {}

    template <bool IsConst>
    constexpr std::string_view GenericMethod<IsConst>::getName() const noexcept {
        return mName;
    }

    template <bool IsConst>
    constexpr bool GenericMethod<IsConst>::isConst() const noexcept {
        return IsConst;
    }


    template <typename ReturnType, typename... Args>
    class GetMethodTypeIdentifiersImpl;

    template <typename ReturnType, typename... Args>
    class GetMethodTypeIdentifiersImpl<ReturnType(Args...)> {
        static constexpr TypeIdentifier returnTypeId = getIdentifier<AdaptPointer<ReturnType>>();
        static constexpr TypeIdentifier argTypeIds[] { getIdentifier<AdaptPointer<Args>>()... };
    public:
        static constexpr MethodTypeIdentifiers value { returnTypeId, std::span<const TypeIdentifier>{ argTypeIds, sizeof...(Args) } };
    };

    template <typename T>
    constexpr MethodTypeIdentifiers getMethodTypeIdentifiers() noexcept {
        return GetMethodTypeIdentifiersImpl<T>::value;
    }

    template <bool IsConst, typename T>
    struct MethodTypeImpl;

    template <bool IsConst, typename ReturnType, typename... Args>
    struct MethodTypeImpl<IsConst, ReturnType(Args...)> {
        using type = const Method<IsConst, ReturnType, Args...>*;
    };

    template <bool IsConst, typename T>
    using MethodType = MethodTypeImpl<IsConst, T>::type;


    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    template <std::size_t N>
    constexpr Method<IsConst, ReturnType, Args...>::Method(const char(& name)[N]) noexcept
        : GenericMethod<IsConst>(name)
    {}

    template <bool IsConst>
    template <typename T>
    const auto* GenericMethod<IsConst>::specialise() const noexcept {
        if (validateTypes(getMethodTypeIdentifiers<T>()))
            return static_cast<MethodType<IsConst, T>>(this);
        return static_cast<MethodType<IsConst, T>>(nullptr);
    }

    template <bool IsConst>
    template <typename ... Args>
    std::expected<BindableTypesVariant, Error> GenericMethod<IsConst>::autoCall(ObjectPointer object, Args&&... args) const {
        return genericCall(object, std::initializer_list<BindableTypesVariant>{ BindableTypesVariant{args}... });
    }

    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    std::expected<BindableTypesVariant, Error> Method<IsConst, ReturnType, Args...>::genericCall(ObjectPointer object, std::span<const BindableTypesVariant> args) const noexcept {
        if (!validateArgumentTypes(args))
            return std::unexpected{ Error::type_mismatch };

        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            if constexpr (std::same_as<ReturnType, void>) {
                return call(object, std::get<AdaptPointer<Args>>(args[I])...).transform([] {
                    return BindableTypesVariant{};
                });
            } else {
                return call(object, std::get<AdaptPointer<Args>>(args[I])...).transform([]<typename T>(T&& val) {
                    return BindableTypesVariant{std::forward<T>(val)};
                });
            }
        }(std::index_sequence_for<Args...>());
    }
    
    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    bool Method<IsConst, ReturnType, Args...>::validateTypes(const MethodTypeIdentifiers& identifiers) const noexcept {
        return getMethodTypeIdentifiers<ReturnType(Args...)>() == identifiers;
    }

    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    constexpr bool Method<IsConst, ReturnType, Args...>::validateArgumentTypes(const std::span<const BindableTypesVariant> args) noexcept {
        if (args.size() != sizeof...(Args))
            return false;
        
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return ((getVariantIdentifier(args[I]) == getIdentifier<AdaptPointer<Args>>()) && ...);
        }(std::index_sequence_for<Args...>{});
    }

    template <typename ClassType, typename ReturnType, typename... Args>
    template <std::size_t N>
    constexpr ConstSpecialisedMethod<ClassType, ReturnType, Args...>::ConstSpecialisedMethod(
        const char (&name)[N],
        const MethodType method
    ) noexcept
        : Method<true, AdaptPointer<std::remove_cvref_t<ReturnType>>, std::remove_cvref_t<AdaptPointer<Args>>...>(name)
        , mMethod(method)
    {}

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::string_view ConstSpecialisedMethod<ClassType, ReturnType, Args...>::getClassName() const noexcept {
        return ClassType::getClassStatic();
    }

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> ConstSpecialisedMethod<ClassType, ReturnType, Args...>::call(const Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept {
        if (const ClassType* o = object_cast<const ClassType*>(object)) {
            if constexpr (std::same_as<ReturnType, void>) {
                std::invoke(mMethod, o, static_cast<Args>(args)...);
                return {};
            } else {
                return { std::invoke(mMethod, o, static_cast<Args>(args)...) };
            }
        }
        return std::unexpected{ Error::object_conversion_failure };
    }

    template <typename ClassType, typename ReturnType, typename... Args>
    template <std::size_t N>
    constexpr MutableSpecialisedMethod<ClassType, ReturnType, Args...>::MutableSpecialisedMethod(
        const char (&name)[N],
        const MethodType method
    ) noexcept
        : Method<false, AdaptPointer<std::remove_cvref_t<ReturnType>>, AdaptPointer<std::remove_cvref_t<Args>>...>(name)
        , mMethod(method)
    {}

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::string_view MutableSpecialisedMethod<ClassType, ReturnType, Args...>::getClassName() const noexcept {
        return ClassType::getClassStatic();
    }

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> MutableSpecialisedMethod<ClassType, ReturnType, Args...>::call(Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept {
        if (ClassType* o = object_cast<ClassType*>(object)) {
            if constexpr (std::same_as<ReturnType, void>) {
                std::invoke(mMethod, o, static_cast<Args>(args)...);
                return {};
            } else {
                return { std::invoke(mMethod, o, static_cast<Args>(args)...) };
            }
        }
        return std::unexpected{ Error::object_conversion_failure };
    }

    template <typename ClassType, typename ReturnType, typename... Args>
    template <std::size_t N>
    constexpr MutableWrapperForConstSpecialisedMethod<ClassType, ReturnType, Args...>::MutableWrapperForConstSpecialisedMethod(
        const char (&name)[N],
        const MethodType method
    ) noexcept
        : Method<false, AdaptPointer<std::remove_cvref_t<ReturnType>>, AdaptPointer<std::remove_cvref_t<Args>>...>(name)
        , mMethod(method)
    {}

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::string_view MutableWrapperForConstSpecialisedMethod<ClassType, ReturnType, Args...>::getClassName() const noexcept {
        return ClassType::getClassStatic();
    }

    template <typename ClassType, typename ReturnType, typename ... Args>
    std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> MutableWrapperForConstSpecialisedMethod<ClassType, ReturnType, Args...>::call(Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept {
        if (const ClassType* o = object_cast<const ClassType*>(object)) {
            if constexpr (std::same_as<ReturnType, void>) {
                std::invoke(mMethod, o, static_cast<Args>(args)...);
                return {};
            } else {
                return { std::invoke(mMethod, o, static_cast<Args>(args)...) };
            }
        }
        return std::unexpected{ Error::object_conversion_failure };
    }
}
