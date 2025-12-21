#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include "CollisionObject2D.hpp"

#include "../../Containers/MoveOnlySmallFunction.hpp"

namespace SPhys {
    class Area2D : public CollisionObject2D {
        template <PhysicsEnvironment2D> friend class PhysicsServer2D;
    public:
        MoveOnlySmallFunction<void(Area2D*)> areaEntered {};
        MoveOnlySmallFunction<void(Area2D*)> areaExited {};

        constexpr Area2D() noexcept;

        [[nodiscard]] constexpr std::span<const Area2D* const> getOverlappingAreas() const noexcept;

        [[nodiscard]] constexpr bool isOverlapping(const Area2D* other) const noexcept;
    private:
        std::vector<Area2D*> mOverlappingAreas {};
    };

    constexpr Area2D::Area2D() noexcept
        : CollisionObject2D(ObjectType2D::area)
#ifdef SPHYS_DEBUG
        , areaEntered([this](const Area2D* other) {
            std::printf("Area2D %p entered Area %p\n", this, other);
        })
        , areaExited([this](const Area2D* other) {
            std::printf("Area2D %p exited Area %p\n", this, other);
        })
#endif
    {}

    constexpr std::span<const Area2D* const> Area2D::getOverlappingAreas() const noexcept {
        return { mOverlappingAreas.begin(), mOverlappingAreas.end() };
    }

    constexpr bool Area2D::isOverlapping(const Area2D* other) const noexcept {
        return std::ranges::contains(mOverlappingAreas, other);
    }
}
