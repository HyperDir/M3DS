#pragma once

#include <functional>
#include <vector>

#include "CollisionObject2D.hpp"

namespace SPhys {
    class Area2D : public CollisionObject2D {
        template <PhysicsEnvironment2D> friend class PhysicsServer2D;
    public:
        std::function<void(Area2D*)> areaEntered = [this](const Area2D* other) {
            std::cout << std::format(
                "Area2D {} entered Area {}",
                static_cast<const void*>(this),
                static_cast<const void*>(other)
            ) << std::endl;
        };
        std::function<void(Area2D*)> areaExited = [this](const Area2D* other) {
            std::cout << std::format(
                "Area2D {} exited Area {}",
                static_cast<const void*>(this),
                static_cast<const void*>(other)
            ) << std::endl;
        };

        explicit constexpr Area2D(
            const Pixels<Vector2>& translation = {},
            float rotation = {},
            const Shape2D& shape = {}
        ) noexcept;

        [[nodiscard]] constexpr std::span<const Area2D* const> getOverlappingAreas() const noexcept;

        [[nodiscard]] constexpr bool isOverlapping(const Area2D* other) const noexcept;
    private:
        std::vector<Area2D*> mOverlappingAreas {};
    };

    constexpr Area2D::Area2D(
        const Pixels<Vector2>& translation,
        const float rotation,
        const Shape2D& shape
    ) noexcept
        : CollisionObject2D(translation, rotation, shape)
    {
        mObjectType = ObjectType2D::area;
    }

    constexpr std::span<const Area2D* const> Area2D::getOverlappingAreas() const noexcept {
        return { mOverlappingAreas.begin(), mOverlappingAreas.end() };
    }

    constexpr bool Area2D::isOverlapping(const Area2D* other) const noexcept {
        return std::ranges::contains(mOverlappingAreas, other);
    }
}
