#pragma once

#include <span>
#include <expected>
#include <string_view>

#include <m3ds/types/TypePack.hpp>
#include <m3ds/utils/ObjectPack.hpp>

namespace M3DS {
    class Object;

    struct MethodTypeIdentifiers {
        TypeIdentifier returnType {};
        std::span<const TypeIdentifier> argTypes {};

        friend constexpr bool operator==(const MethodTypeIdentifiers& lhs, const MethodTypeIdentifiers& rhs) noexcept;
    };

    template <typename T>
    struct AdaptPointerImpl {
        using Type = T;
    };

    template <std::derived_from<Object> T>
    struct AdaptPointerImpl<T*> {
        using Type = Object*;
    };

    template <std::derived_from<Object> T>
    struct AdaptPointerImpl<const T*> {
        using Type = const Object*;
    };

    template <typename T>
    using AdaptPointer = AdaptPointerImpl<T>::Type;

    template <bool IsConst>
    class GenericMethod;

    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    class Method;

    using ConstGenericMethod = GenericMethod<true>;
    using MutableGenericMethod = GenericMethod<false>;

    template <typename ReturnType, typename... Args>
    using ConstMethod = Method<true, ReturnType, Args...>;
    template <typename ReturnType, typename... Args>
    using MutableMethod = Method<false, ReturnType, Args...>;

    template <bool IsConst>
    class GenericMethod {
        using ObjectPointer = std::conditional_t<IsConst, const Object*, Object*>;
    public:
        template <std::size_t N>
        [[nodiscard]] explicit consteval GenericMethod(const char (&name)[N]) noexcept;

        virtual ~GenericMethod() = default;

        template <typename T>
        const auto* specialise() const noexcept;

        [[nodiscard]] constexpr std::string_view getName() const noexcept;
        [[nodiscard]] constexpr bool isConst() const noexcept;

        [[nodiscard]] virtual std::string_view getClassName() const noexcept = 0;

        [[nodiscard]] virtual std::expected<BindableTypesVariant, Error> genericCall(ObjectPointer object, std::span<const BindableTypesVariant> args) const noexcept = 0;

        template <typename... Args>
        [[nodiscard]] std::expected<BindableTypesVariant, Error> autoCall(ObjectPointer object, Args&&... args) const;
    protected:
        [[nodiscard]] virtual bool validateTypes(const MethodTypeIdentifiers& identifiers) const noexcept = 0;
    private:
        std::string_view mName {};
    };

    template <bool IsConst, BindableType ReturnType, BindableType... Args>
    class Method : public GenericMethod<IsConst> {
        using ObjectPointer = std::conditional_t<IsConst, const Object*, Object*>;
    public:
        template <std::size_t N>
        [[nodiscard]] explicit consteval Method(const char (&name)[N]) noexcept;

        [[nodiscard]] std::expected<BindableTypesVariant, Error> genericCall(ObjectPointer object, std::span<const BindableTypesVariant> args) const noexcept final;

        [[nodiscard]] virtual std::expected<std::remove_cvref_t<ReturnType>, Error> call(ObjectPointer object, PassEfficiently<Args>... args) const noexcept = 0;
    protected:
        [[nodiscard]] bool validateTypes(const MethodTypeIdentifiers& identifiers) const noexcept final;
    private:
        static constexpr bool validateArgumentTypes(std::span<const BindableTypesVariant> args) noexcept;
    };


    template <typename ClassType, typename ReturnType, typename... Args>
    class ConstSpecialisedMethod final : public Method<true, AdaptPointer<std::remove_cvref_t<ReturnType>>, AdaptPointer<std::remove_cvref_t<Args>>...> {
        using MethodType = ReturnType(ClassType::*)(Args...) const;
    public:
        template <std::size_t N>
        consteval ConstSpecialisedMethod(
            const char (&name)[N],
            MethodType method
        ) noexcept;

        [[nodiscard]] std::string_view getClassName() const noexcept override;
        [[nodiscard]] std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> call(const Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept override;
    private:
        MethodType mMethod {};
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    class MutableSpecialisedMethod final : public Method<false, AdaptPointer<std::remove_cvref_t<ReturnType>>, AdaptPointer<std::remove_cvref_t<Args>>...> {
        using MethodType = ReturnType(ClassType::*)(Args...);
    public:
        template <std::size_t N>
        consteval MutableSpecialisedMethod(
            const char (&name)[N],
            MethodType method
        ) noexcept;

        [[nodiscard]] std::string_view getClassName() const noexcept override;
        [[nodiscard]] std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> call(Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept override;
    private:
        MethodType mMethod {};
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    class MutableWrapperForConstSpecialisedMethod final : public Method<false, AdaptPointer<std::remove_cvref_t<ReturnType>>, AdaptPointer<std::remove_cvref_t<Args>>...> {
        using MethodType = ReturnType(ClassType::*)(Args...) const;
    public:

        template <std::size_t N>
        consteval MutableWrapperForConstSpecialisedMethod(
            const char (&name)[N],
            MethodType method
        ) noexcept;

        [[nodiscard]] std::string_view getClassName() const noexcept override;
        [[nodiscard]] std::expected<AdaptPointer<std::remove_cvref_t<ReturnType>>, Error> call(Object* object, PassEfficiently<AdaptPointer<Args>>... args) const noexcept override;
    private:
        MethodType mMethod {};
    };



    struct BoundMethodPair {
        const ConstGenericMethod* constMethod {};
        const MutableGenericMethod* mutableMethod {};
    };

    template <typename T, typename U>
    struct MethodPair {
        T constMethod {};
        U mutableMethod {};

        constexpr BoundMethodPair get() const noexcept {
            return {
                &constMethod,
                &mutableMethod
            };
        }
    };

    template <typename T>
    struct SingleMethod {
        T mutableMethod {};

        constexpr BoundMethodPair get() const noexcept {
            return {
                nullptr,
                &mutableMethod
            };
        }
    };

    template <typename... Pairs>
    class MethodPack {
        std::tuple<Pairs...> mMethods;
        std::array<BoundMethodPair, sizeof...(Pairs)> mArray;

        template <std::size_t... Is>
        [[nodiscard]] explicit consteval MethodPack(Pairs&&... methods, std::index_sequence<Is...>)
            : mMethods(std::forward<Pairs>(methods)...)
            , mArray{std::get<Is>(mMethods).get()...}
        {}
    public:
        [[nodiscard]] explicit consteval MethodPack(Pairs&&... methods) noexcept
            : MethodPack(std::forward<Pairs>(methods)..., std::index_sequence_for<Pairs...>())
        {}

        [[nodiscard]] constexpr std::span<const BoundMethodPair> getMethods() const noexcept {
            return mArray;
        }
    };
}
