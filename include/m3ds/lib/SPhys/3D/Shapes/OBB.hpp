#pragma once

#include "Projection3D.hpp"
#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector3.hpp"
#include "../../Spatial/Quaternion.hpp"

namespace SPhys {
    struct OBB {
        Metres<Vector3> origin {};
        Metres<Vector3> halfExtents {};
        Quaternion rotation {};

        constexpr void setTranslation(const Vector3& to) noexcept;
        [[nodiscard]] constexpr const Vector3& getTranslation() const noexcept;

        constexpr void rotate(const Quaternion& by) noexcept;

        constexpr void setRotation(const Quaternion& to) noexcept;
        [[nodiscard]] constexpr const Quaternion& getRotation() const noexcept;

        [[nodiscard]] constexpr Projection3D project(const Vector3& axis) const noexcept;
        [[nodiscard]] constexpr std::array<Vector3, 3> getSeparationAxes() const noexcept;
        [[nodiscard]] constexpr std::array<Vector3, 3> getSeparationEdges() const noexcept;
        [[nodiscard]] constexpr Vector3 getClosestPoint(const Vector3& to) const noexcept;

        [[nodiscard]] constexpr BoundingBox3D getBoundingBox() const noexcept;
    };

    constexpr void OBB::setTranslation(const Vector3& to) noexcept {
        origin = to;
    }

    [[nodiscard]] constexpr const Vector3& OBB::getTranslation() const noexcept {
        return origin;
    }

    constexpr void OBB::rotate(const Quaternion& by) noexcept {
        rotation = by * rotation;
    }

    constexpr void OBB::setRotation(const Quaternion& to) noexcept {
        rotation = to;
    }

    constexpr const Quaternion& OBB::getRotation() const noexcept {
        return rotation;
    }

    constexpr Projection3D OBB::project(const Vector3& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);

        const Vector3 localX = rotation.rotate(Vector3{1, 0, 0}) * halfExtents.x;
        const Vector3 localY = rotation.rotate(Vector3{0, 1, 0}) * halfExtents.y;
        const Vector3 localZ = rotation.rotate(Vector3{0, 0, 1}) * halfExtents.z;

        const float radius = std::abs(axis.dot(localX))
                           + std::abs(axis.dot(localY))
                           + std::abs(axis.dot(localZ));

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr std::array<Vector3, 3> OBB::getSeparationAxes() const noexcept {
        return {
            Vector3{1, 0, 0}.rotated(rotation),
            Vector3{0, 1, 0}.rotated(rotation),
            Vector3{0, 0, 1}.rotated(rotation)
        };
    }

    constexpr std::array<Vector3, 3> OBB::getSeparationEdges() const noexcept {
        return getSeparationAxes();
    }

    constexpr Vector3 OBB::getClosestPoint(const Vector3& to) const noexcept {
        return origin + (to - origin).rotated(rotation.inverse()).clamp(-halfExtents, halfExtents).rotated(rotation);
    }

    constexpr BoundingBox3D OBB::getBoundingBox() const noexcept {
        const Vector3 u = Vector3{1, 0, 0}.rotated(rotation);
        const Vector3 v = Vector3{0, 1, 0}.rotated(rotation);
        const Vector3 w = Vector3{0, 0, 1}.rotated(rotation);

        const Vector3 alignedExtents = {
            std::abs(u.x) * halfExtents.x + std::abs(v.x) * halfExtents.y + std::abs(w.x) * halfExtents.z,
            std::abs(u.y) * halfExtents.x + std::abs(v.y) * halfExtents.y + std::abs(w.y) * halfExtents.z,
            std::abs(u.z) * halfExtents.x + std::abs(v.z) * halfExtents.y + std::abs(w.z) * halfExtents.z
        };

        return { origin - alignedExtents, origin + alignedExtents };
    }
}
