#pragma once

#include "../Containers/Hive.hpp"

#include "../3D/PhysicsEnvironment3D.hpp"
#include "../2D/PhysicsEnvironment2D.hpp"

namespace SPhys {
    template <typename T>
    class Accessor {
        template <PhysicsEnvironment2D> friend class PhysicsServer2D;
        template <PhysicsEnvironment3D> friend class PhysicsServer3D;

        HiveIterator<T> mIterator {};
    public:
        [[nodiscard]] constexpr Accessor() noexcept = default;
        [[nodiscard]] explicit constexpr Accessor(HiveIterator<T> iterator) noexcept : mIterator(iterator) {}

        [[nodiscard]] constexpr T& value() const noexcept { return *mIterator; }
        [[nodiscard]] constexpr T* get() const noexcept { return &value(); }

        [[nodiscard]] constexpr T* operator->() const noexcept { return get(); }
        [[nodiscard]] constexpr T& operator*() const noexcept { return *get(); }
    };
}
