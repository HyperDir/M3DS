#pragma once

#include <exception>

#include <m3ds/containers/UninitialisedStorage.hpp>

namespace M3DS {
    class BadStrongOptionalAccess final : public std::exception {
    public:
        const char* what() const noexcept override { return "Bad StrongOptional Access!"; }
    };

    template <typename T>
    requires (!std::is_reference_v<T>)
    class StrongOptional {
        friend T;
    public:
        [[nodiscard]] constexpr StrongOptional() noexcept = default;

        [[nodiscard]] constexpr StrongOptional(const StrongOptional& other) requires std::is_copy_constructible_v<T>;
        constexpr StrongOptional& operator=(const StrongOptional& other) requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T>;

        [[nodiscard]] constexpr StrongOptional(StrongOptional&& other) noexcept;
        constexpr StrongOptional& operator=(StrongOptional&& other) noexcept;

        explicit StrongOptional(T&&) requires (!std::is_trivially_move_constructible_v<T>) = delete("Would leave other T in moved-from state!");

        constexpr ~StrongOptional() noexcept requires (std::is_trivially_destructible_v<T>) = default;
        constexpr ~StrongOptional() noexcept requires (!std::is_trivially_destructible_v<T>);

        [[nodiscard]] constexpr bool has_value() const noexcept;
        [[nodiscard]] explicit constexpr operator bool() const noexcept;

        [[nodiscard]] constexpr T& value();
        [[nodiscard]] constexpr const T& value() const;

        [[nodiscard]] constexpr T& operator*() noexcept;
        [[nodiscard]] constexpr const T& operator*() const noexcept;

        [[nodiscard]] constexpr T* operator->() noexcept;
        [[nodiscard]] constexpr const T* operator->() const noexcept;
    private:
        template <typename... Args>
        [[nodiscard]] constexpr StrongOptional(Args&&... args);


        template <typename... Args>
        constexpr void construct(Args&&... args);
        constexpr void destroy() noexcept;

        UninitialisedStorage<T> mStorage;
        bool mHasValue {};
    };
}




/* Implementation */
namespace M3DS {
    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>::StrongOptional(const StrongOptional& other) requires std::is_copy_constructible_v<T> {
        if (!other.has_value())
            return;

        construct(other);
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>& StrongOptional<T>::operator=(const StrongOptional& other) requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> {
        if (mHasValue) {
            mStorage.object = other.mStorage.object;
        } else {
            construct(other);
        }

        return *this;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>::StrongOptional(StrongOptional&& other) noexcept {
        if (!other.mHasValue)
            return;

        construct(std::move(other));
        other.destroy();
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>& StrongOptional<T>::operator=(StrongOptional&& other) noexcept {
        if (!other.mHasValue)
            return *this;

        if (mHasValue)
            mStorage.object = std::move(other.mStorage.object);
        else
            construct(std::move(other));

        other.destroy();
        return *this;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>::~StrongOptional() noexcept requires (!std::is_trivially_destructible_v<T>) {
        if (mHasValue)
            destroy();
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr bool StrongOptional<T>::has_value() const noexcept {
        return mHasValue;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr StrongOptional<T>::operator bool() const noexcept {
        return mHasValue;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr T& StrongOptional<T>::value() {
        if (!mHasValue) {
#if __cpp_exceptions
            throw BadStrongOptionalAccess{};
#else
            std::terminate();
#endif
        }

        return mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr const T& StrongOptional<T>::value() const {
        if (!mHasValue) {
#if __cpp_exceptions
            throw BadStrongOptionalAccess{};
#else
            std::terminate();
#endif
        }

        return mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr T& StrongOptional<T>::operator*() noexcept {
        return mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr const T& StrongOptional<T>::operator*() const noexcept {
        return mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr T* StrongOptional<T>::operator->() noexcept {
        return &mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr const T* StrongOptional<T>::operator->() const noexcept {
        return &mStorage.object;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    template <typename ... Args>
    constexpr StrongOptional<T>::StrongOptional(Args&&... args) {
        construct(std::forward<Args>(args)...);
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    template <typename ... Args>
    constexpr void StrongOptional<T>::construct(Args&&... args) {
        new (&mStorage.object) T(std::forward<Args>(args)...);
        mHasValue = true;
    }

    template <typename T>
    requires (!std::is_reference_v<T>)
    constexpr void StrongOptional<T>::destroy() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            mStorage.object.~T();
        }
        mHasValue = false;
    }
}
