#pragma once

#include <tuple>
#include <array>

namespace M3DS {
    template <typename BaseClass, typename... Args>
    class ObjectPack;

    template <typename BaseClass, typename... Args>
    class ObjectPack<BaseClass(Args...)> final {
        std::tuple<Args...> objects {};
    public:
        constexpr ObjectPack(Args&&... args);

        constexpr auto getElements();
        constexpr auto getElements() const;
    };

    template <typename BaseClass, typename... Args>
    constexpr ObjectPack<BaseClass(Args...)>::ObjectPack(Args&&... args)
        : objects(std::forward<Args>(args)...)
    {}

    template <typename BaseClass, typename... Args>
    constexpr auto ObjectPack<BaseClass(Args...)>::getElements() {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::array<BaseClass*, sizeof...(Args)>{ &std::get<I>(objects)... };
        }(std::index_sequence_for<Args...>{});
    }

    template <typename BaseClass, typename... Args>
    constexpr auto ObjectPack<BaseClass(Args...)>::getElements() const {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::array<const BaseClass*, sizeof...(Args)>{ &std::get<I>(objects)... };
        }(std::index_sequence_for<Args...>{});
    }

    template <typename T, typename... Args>
    constexpr ObjectPack<T(Args...)> createObjectPack(Args&&... args) noexcept {
        return { std::forward<Args>(args)... };
    }
}
