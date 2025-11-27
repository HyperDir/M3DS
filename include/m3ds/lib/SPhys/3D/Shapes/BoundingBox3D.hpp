#pragma once

#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector3.hpp"

namespace SPhys {
    struct BoundingBox3D {
        Metres<Vector3> min {};
        Metres<Vector3> max {};

        [[nodiscard]] constexpr bool isOverlapping(const BoundingBox3D& other) const noexcept;

        [[nodiscard]] constexpr BoundingBox3D expand(const Vector3& by) const noexcept;
    };

    constexpr bool BoundingBox3D::isOverlapping(const BoundingBox3D& other) const noexcept {
        return  (min.x <= other.max.x && max.x >= other.min.x) &&
                (min.y <= other.max.y && max.y >= other.min.y) &&
                (min.z <= other.max.z && max.z >= other.min.z);
    }

    constexpr BoundingBox3D BoundingBox3D::expand(const Vector3& by) const noexcept {
        return {
            SPhys::min(min, min + by),
            SPhys::max(max, max + by)
        };
    }
}
