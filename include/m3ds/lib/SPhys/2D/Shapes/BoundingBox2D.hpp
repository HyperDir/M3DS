#pragma once

#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector2.hpp"

namespace SPhys {
    struct BoundingBox2D {
        Pixels<Vector2> min {};
        Pixels<Vector2> max {};

        [[nodiscard]] constexpr bool isOverlapping(const BoundingBox2D& other) const noexcept;

        [[nodiscard]] constexpr BoundingBox2D expand(const Vector2& by) const noexcept;
    };

    constexpr bool BoundingBox2D::isOverlapping(const BoundingBox2D& other) const noexcept {
        return  (min.x <= other.max.x && max.x >= other.min.x) &&
                (min.y <= other.max.y && max.y >= other.min.y);
    }

    constexpr BoundingBox2D BoundingBox2D::expand(const Vector2& by) const noexcept {
        return {
            SPhys::min(min, min + by),
            SPhys::max(max, max + by)
        };
    }
}
