#pragma once

#include <type_traits>

namespace M3DS {
    template <typename T>
    requires (!std::is_reference_v<T>)
    union UninitialisedStorage {
        constexpr UninitialisedStorage() noexcept requires (std::is_trivially_constructible_v<T>) = default;
        constexpr UninitialisedStorage() noexcept requires (!std::is_trivially_constructible_v<T>) {}

        UninitialisedStorage(const UninitialisedStorage&) = delete;
        UninitialisedStorage& operator=(const UninitialisedStorage&) = delete;

        UninitialisedStorage(UninitialisedStorage&&) = delete;
        UninitialisedStorage& operator=(UninitialisedStorage&&) = delete;

        constexpr ~UninitialisedStorage() noexcept requires (std::is_trivially_destructible_v<T>) = default;
        constexpr ~UninitialisedStorage() noexcept requires (!std::is_trivially_destructible_v<T>) {}

        T object;
    };
}
