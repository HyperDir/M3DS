#pragma once

#include <functional>
#include <vector>

#include "CollisionObject3D.hpp"

namespace SPhys {
    class Area3D : public CollisionObject3D {
        template <PhysicsEnvironment3D> friend class PhysicsServer3D;
    public:
        std::function<void(Area3D*)> areaEntered = [this](const Area3D* other) {
            std::cout << std::format(
                "Area3D {} entered Area {}",
                static_cast<const void*>(this),
                static_cast<const void*>(other)
            ) << std::endl;
        };
        std::function<void(Area3D*)> areaExited = [this](const Area3D* other) {
            std::cout << std::format(
                "Area3D {} exited Area {}",
                static_cast<const void*>(this),
                static_cast<const void*>(other)
            ) << std::endl;
        };

        explicit constexpr Area3D(
            const Metres<Vector3>& translation = {},
            const Quaternion& rotation = {},
            const Shape3D& shape = {}
        ) noexcept;

        [[nodiscard]] constexpr std::span<const Area3D* const> getOverlappingAreas() const noexcept;

        [[nodiscard]] constexpr bool isOverlapping(const Area3D* other) const noexcept;
    private:
        std::vector<Area3D*> mOverlappingAreas {};
    };

    constexpr Area3D::Area3D(
        const Metres<Vector3>& translation,
        const Quaternion& rotation,
        const Shape3D& shape
    ) noexcept
        : CollisionObject3D(translation, rotation, shape)
    {
        mObjectType = ObjectType3D::area;
    }

    constexpr std::span<const Area3D* const> Area3D::getOverlappingAreas() const noexcept {
        return { mOverlappingAreas.begin(), mOverlappingAreas.end() };
    }

    constexpr bool Area3D::isOverlapping(const Area3D* other) const noexcept {
        return std::ranges::contains(mOverlappingAreas, other);
    }
}
