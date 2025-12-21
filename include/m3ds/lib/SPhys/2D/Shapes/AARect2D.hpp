#pragma once

#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector2.hpp"

#include "../Projection2D.hpp"
#include "BoundingBox2D.hpp"

namespace SPhys {
    struct AARect2D {
        Pixels<Vector2> origin {};
        Pixels<Vector2> halfExtents {};

        constexpr void setTranslation(const Vector2& to) noexcept;
        [[nodiscard]] constexpr const Vector2& getTranslation() const noexcept;

        [[nodiscard]] constexpr Projection2D project(const Vector2& axis) const noexcept;
        [[nodiscard]] constexpr const std::array<Vector2, 2>& getSeparationAxes() const noexcept;
        [[nodiscard]] constexpr Vector2 getClosestPoint(const Vector2& to) const noexcept;

        [[nodiscard]] constexpr BoundingBox2D getBoundingBox() const noexcept;
    };

    constexpr void AARect2D::setTranslation(const Vector2& to) noexcept {
        origin = to;
    }

    constexpr const Vector2& AARect2D::getTranslation() const noexcept {
        return origin;
    }

    constexpr Projection2D AARect2D::project(const Vector2& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);
        const float radius = axis.abs().dot(halfExtents);

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr const std::array<Vector2, 2>& AARect2D::getSeparationAxes() const noexcept {
        static constexpr std::array<Vector2, 2> separationAxes {{
                { 1, 0 },
                { 0, 1 }
        }};
        return separationAxes;
    }

    constexpr Vector2 AARect2D::getClosestPoint(const Vector2& to) const noexcept {
        return origin + (to - origin).clamp(-halfExtents, halfExtents);
    }

    constexpr BoundingBox2D AARect2D::getBoundingBox() const noexcept {
        return { origin - halfExtents, origin + halfExtents };
    }
}
