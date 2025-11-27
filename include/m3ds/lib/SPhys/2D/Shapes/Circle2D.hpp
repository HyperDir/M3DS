#pragma once

#include "BoundingBox2D.hpp"
#include "Projection2D.hpp"
#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector2.hpp"

namespace SPhys {
    struct Circle2D {
        Pixels<Vector2> origin {};
        Pixels<float> radius {};

        constexpr void setTranslation(const Vector2& to) noexcept;
        [[nodiscard]] constexpr const Vector2& getTranslation() const noexcept;

        [[nodiscard]] constexpr Projection2D project(const Vector2& axis) const noexcept;

        [[nodiscard]] constexpr BoundingBox2D getBoundingBox() const noexcept;
    };

    constexpr void Circle2D::setTranslation(const Vector2& to) noexcept {
        origin = to;
    }

    constexpr const Vector2& Circle2D::getTranslation() const noexcept {
        return origin;
    }

    constexpr Projection2D Circle2D::project(const Vector2& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr BoundingBox2D Circle2D::getBoundingBox() const noexcept {
        return { origin - Vector2{radius}, origin + Vector2{radius} };
    }
}
