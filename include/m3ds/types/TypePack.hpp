#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include <m3ds/types/NodePath.hpp>

#include <m3ds/spatial/Matrix4x4.hpp>
#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Vector3.hpp>
#include <m3ds/spatial/Quaternion.hpp>
#include <m3ds/spatial/Transform2D.hpp>

#include <m3ds/utils/Debug.hpp>

namespace M3DS {
    template <typename... Types>
    struct TypePack {
        template <template <typename...> typename T>
        using apply = T<Types...>;

        template <template <typename> typename Wrapper>
        using transform = TypePack<Wrapper<Types>...>;

        template <typename... Ts>
        using append = TypePack<Types..., Ts...>;

        template <typename... Ts>
        using prepend = TypePack<Ts..., Types...>;
    private:
        template <typename T, size_t i>
        static consteval std::size_t indexOfImpl() noexcept {
            static_assert(sizeof...(Types) > 0, "TypePack has no members!");
            return 0;
        }

        template <typename T, size_t i, typename First, typename... Rest>
        static consteval std::size_t indexOfImpl() noexcept {
            if constexpr (std::same_as<T, First>)
                return i;
            else {
                static_assert(sizeof...(Rest) > 0, "Type not in TypePack!");
                return indexOfImpl<T, i + 1, Rest...>();
            }
        }
    public:
        template <typename T>
        static consteval std::size_t indexOf() noexcept {
            return indexOfImpl<T, 0, Types...>();
        }

        template <std::size_t I>
        using TypeAt = Types...[I];

        static consteval std::size_t size() noexcept {
            return sizeof...(Types);
        }

        template <typename T>
        static consteval bool contains() noexcept {
            return (std::same_as<T, Types> || ...);
        }

        template <typename T>
        static consteval bool containsConversionOf() noexcept {
            return (std::convertible_to<T, Types> || ...);
        }
    };

    using AnimationTypes = TypePack<
        bool,
        std::uint8_t,
        std::int32_t,
        std::uint32_t,
        std::size_t,
        float,
        Vector2,
        Vector2i,
        Vector3,
        Vector3i,
        Quaternion,
        std::string
    >;

    class BaseScript;
    class Object;

    using BindableTypes = AnimationTypes::append<
        Transform2D,
        Matrix4x4,
        NodePath,
        Object*,
        const Object*,
        BaseScript*,
        const BaseScript*,
        // std::shared_ptr<Resource>,
        std::string_view
    >;

    using BindableTypesVariant = BindableTypes::prepend<std::monostate>::apply<std::variant>;

    template <typename T>
    concept BindableType = std::same_as<T, void> || BindableTypes::contains<std::remove_cvref_t<T>>() || std::derived_from<std::remove_const_t<std::remove_pointer_t<T>>, Object>;

    using TypeIdentifier = std::uint8_t;

    template <typename T>
    constexpr TypeIdentifier getIdentifier() noexcept {
        if constexpr (std::same_as<void, T>) return 0;
        else return static_cast<TypeIdentifier>(BindableTypes::indexOf<T>() + 1); // adjust for monostate
    }

    constexpr TypeIdentifier getVariantIdentifier(const BindableTypesVariant& var) noexcept {
        return static_cast<TypeIdentifier>(var.index());
    }

    constexpr bool isValidIdentifier(const std::uint32_t identifier) noexcept {
        return identifier < BindableTypes::size();
    }

    // TODO: Jump table?
    template <typename Callable>
    constexpr auto visit(const std::uint32_t identifier, Callable callable) {
#define M3DS_VISIT_IMPL(type) case getIdentifier<type>(): return callable.template operator()<type>()
        switch (identifier) {
            M3DS_VISIT_IMPL(bool);
            M3DS_VISIT_IMPL(std::uint8_t);
            M3DS_VISIT_IMPL(std::int32_t);
            M3DS_VISIT_IMPL(std::uint32_t);
            M3DS_VISIT_IMPL(std::size_t);
            M3DS_VISIT_IMPL(float);
            M3DS_VISIT_IMPL(Vector2);
            M3DS_VISIT_IMPL(Vector2i);
            M3DS_VISIT_IMPL(Vector3);
            M3DS_VISIT_IMPL(Vector3i);
            M3DS_VISIT_IMPL(Quaternion);
            M3DS_VISIT_IMPL(std::string);

            M3DS_VISIT_IMPL(Transform2D);
            M3DS_VISIT_IMPL(Matrix4x4);
            M3DS_VISIT_IMPL(NodePath);
            M3DS_VISIT_IMPL(Object*);
            M3DS_VISIT_IMPL(const Object*);
            M3DS_VISIT_IMPL(BaseScript*);
            M3DS_VISIT_IMPL(const BaseScript*);
        default:
            Debug::terminate("Reached end of M3DS::visit! Identifier: {}", identifier);
        }
#undef M3DS_VISIT_IMPL
    }

    template <typename T>
    using PassEfficiently = std::conditional_t<
        (sizeof(std::remove_cvref_t<T>) > sizeof(void*) * 2), 
        const std::remove_cvref_t<T>&, 
        std::remove_reference_t<T>
    >;

    template <typename Variant, std::size_t I = 0>
    Variant variantFromIndex(std::size_t idx) {
        if constexpr (I >= std::variant_size_v<Variant>) {
            Debug::terminate("Attempted to get index {} on variant of size {}", idx, std::variant_size_v<Variant>);
        } else {
            if (idx == I)
                return Variant{std::in_place_index<I>};
            return variantFromIndex<Variant, I + 1>(idx);
        }
    }
}
