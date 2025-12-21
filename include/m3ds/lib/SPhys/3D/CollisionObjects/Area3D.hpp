#pragma once

#include <vector>

#include "CollisionObject3D.hpp"

#include "../../Containers/MoveOnlySmallFunction.hpp"

namespace SPhys {
    class Area3D : public CollisionObject3D {
        template <PhysicsEnvironment3D> friend class PhysicsServer3D;
    public:
        MoveOnlySmallFunction<void(Area3D*)> areaEntered {};
        MoveOnlySmallFunction<void(Area3D*)> areaExited {};

        constexpr Area3D();

        [[nodiscard]] constexpr std::span<const Area3D* const> getOverlappingAreas() const noexcept;

        [[nodiscard]] constexpr bool isOverlapping(const Area3D* other) const noexcept;
    private:
        std::vector<Area3D*> mOverlappingAreas {};
    };

    constexpr Area3D::Area3D()
        : CollisionObject3D(ObjectType3D::area)
#ifdef SPHYS_DEBUG
        , areaEntered([this](const Area3D* other) {
            std::printf("Area3D %p entered Area %p\n", this, other);
        })
        , areaExited([this](const Area3D* other) {
            std::printf("Area3D %p exited Area %p\n", this, other);
        })
#endif
    {}

    constexpr std::span<const Area3D* const> Area3D::getOverlappingAreas() const noexcept {
        return { mOverlappingAreas.begin(), mOverlappingAreas.end() };
    }

    constexpr bool Area3D::isOverlapping(const Area3D* other) const noexcept {
        return std::ranges::contains(mOverlappingAreas, other);
    }
}
