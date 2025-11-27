#pragma once

#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector3.hpp"

#include "Projection3D.hpp"
#include "BoundingBox3D.hpp"

namespace SPhys {
    struct AABB {
        Metres<Vector3> origin {};
        Metres<Vector3> halfExtents {};

        constexpr void setTranslation(const Vector3& to) noexcept;
        [[nodiscard]] constexpr const Vector3& getTranslation() const noexcept;

        [[nodiscard]] constexpr Projection3D project(const Vector3& axis) const noexcept;
        [[nodiscard]] constexpr const std::array<Vector3, 3>& getSeparationAxes() const noexcept;
        [[nodiscard]] constexpr const std::array<Vector3, 3>& getSeparationEdges() const noexcept;
        [[nodiscard]] constexpr Vector3 getClosestPoint(const Vector3& to) const noexcept;

        [[nodiscard]] constexpr BoundingBox3D getBoundingBox() const noexcept;
    };

    constexpr void AABB::setTranslation(const Vector3& to) noexcept {
        origin = to;
    }

    [[nodiscard]] constexpr const Vector3& AABB::getTranslation() const noexcept {
        return origin;
    }
    constexpr Projection3D AABB::project(const Vector3& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);
        const float radius = axis.abs().dot(halfExtents);

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr const std::array<Vector3, 3>& AABB::getSeparationAxes() const noexcept {
        static constexpr std::array<Vector3, 3> axes {{
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
        }};
        return axes;
    }

    constexpr const std::array<Vector3, 3>& AABB::getSeparationEdges() const noexcept {
        return getSeparationAxes();
    }

    constexpr Vector3 AABB::getClosestPoint(const Vector3& to) const noexcept {
        return origin + (to - origin).clamp(-halfExtents, halfExtents);
    }

    constexpr BoundingBox3D AABB::getBoundingBox() const noexcept {
        return { origin - halfExtents, origin + halfExtents };
    }
}
