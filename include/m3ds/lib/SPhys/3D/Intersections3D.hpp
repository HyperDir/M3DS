#pragma once

#include <algorithm>

#include "Shape3D.hpp"

namespace SPhys {
    // Minimum Translation Vector (shortest move distance required to resolve collision)
    struct MTV3D {
        Vector3 normal {};
        float magnitude {};

        [[nodiscard]] constexpr Vector3 get() const noexcept;
    };

    constexpr std::optional<MTV3D> separatingAxisTest(
        const SeparatingAxisShapeType3D auto& lhs,
        const SeparatingAxisShapeType3D auto& rhs
    ) noexcept;

    constexpr bool isIntersecting(
        const SeparatingAxisShapeType3D auto& lhs,
        const SeparatingAxisShapeType3D auto& rhs
    ) noexcept;
}

/* Implementation */
namespace SPhys {
    constexpr Vector3 MTV3D::get() const noexcept {
        return normal * magnitude;
    }

    constexpr std::optional<MTV3D> separatingAxisTest(const Sphere& lhs, const Sphere& rhs) noexcept {
        const Vector3 delta = lhs.origin - rhs.origin;
        const float distanceSquared = delta.lengthSquared();
        const float combinedRadius = lhs.radius + rhs.radius;

        if (distanceSquared >= combinedRadius * combinedRadius) return {};

        // If origins overlap, push out along X-axis
        if (std::abs(distanceSquared) < 1e-6f) {
            return { MTV3D{{&lhs > &rhs ? 1.f : -1.f, 0, 0}, combinedRadius} };
        }

        const float distance = std::sqrt(distanceSquared);
        const float overlap = combinedRadius - distance;

        const Vector3 collisionNormal = delta / distance;
        return { MTV3D{collisionNormal, overlap} };
    }

    constexpr std::optional<MTV3D> separatingAxisTest(
        const SeparatingAxisShapeType3D auto& lhs,
        const SeparatingAxisShapeType3D auto& rhs
    ) noexcept {
        return separatingAxisTest(
            lhs.getSeparationAxes(),
            rhs.getSeparationAxes(),
            lhs.getSeparationEdges(),
            rhs.getSeparationEdges(),
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV3D> separatingAxisTest(
        const std::span<const Vector3> axes1,
        const std::span<const Vector3> axes2,
        const std::span<const Vector3> edges1,
        const std::span<const Vector3> edges2,
        const ShapeType3D auto& lhs,
        const ShapeType3D auto& rhs
    ) noexcept {
        // https://dyn4j.org/3010/01/
        float minOverlap = 1e10f;
        const Vector3* smallestAxis {};

        bool negate = false;

        #define AXIS_TEST_3D(axis) {                            \
            const Projection3D p1 = lhs.project(axis);          \
            const Projection3D p2 = rhs.project(axis);          \
            float overlap = p1.getOverlap(p2);                  \
            if (overlap <= 0)                                   \
                return {};                                      \
                                                                \
            if (p1.contains(p2) || p2.contains(p1)) {           \
                const float mins = std::abs(p1.from - p2.from); \
                const float maxes = std::abs(p1.to - p2.to);    \
                                                                \
                if (mins < maxes) overlap += mins;              \
                else overlap += maxes;                          \
            }                                                   \
                                                                \
            if (overlap < minOverlap) {                         \
                minOverlap = overlap;                           \
                smallestAxis = &axis;                           \
                                                                \
                negate = p2.from + p2.to > p1.from + p1.to;     \
            }                                                   \
        }

        for (const auto& axis : axes1)
            AXIS_TEST_3D(axis);

        for (const auto& axis : axes2)
            AXIS_TEST_3D(axis);

        for (const auto& edge1 : edges1) {
            for (const auto& edge2 : edges2) {
                Vector3 product = edge1.cross(edge2);
                if (product.lengthSquared() > 1e-6f) {
                    product = product.normalise();
                    AXIS_TEST_3D(product);
                }
            }
        }

        if (!smallestAxis) return {};

        return { MTV3D{ negate ? -*smallestAxis : *smallestAxis, minOverlap } };
    }

    constexpr std::optional<MTV3D> separatingAxisTest(const Sphere& lhs, const SeparatingAxisShapeType3D auto& rhs) noexcept {
        const Vector3 closestPoint = rhs.getClosestPoint(lhs.origin);

        if (closestPoint.distanceSquaredTo(lhs.origin) < 1e-6f) {
            return separatingAxisTest(
                rhs.getSeparationAxes(),
                {},
                {},
                {},
                lhs,
                rhs
            );
        }

        return separatingAxisTest(
            {(lhs.origin - closestPoint).normalise()},
            rhs.getSeparationAxes(),
            {},
            {},
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV3D> separatingAxisTest(const SeparatingAxisShapeType3D auto& lhs, const Sphere& rhs) noexcept {
        const Vector3 closestPoint = lhs.getClosestPoint(rhs.origin);

        if (closestPoint.distanceSquaredTo(rhs.origin) < 1e-6f) {
            return separatingAxisTest(
                lhs.getSeparationAxes(),
                {},
                {},
                {},
                lhs,
                rhs
            );
        }

        return separatingAxisTest(
            lhs.getSeparationAxes(),
            {(rhs.origin - closestPoint).normalise()},
            {},
            {},
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV3D> separatingAxisTest(const AABB& lhs, const AABB& rhs) noexcept {
        return separatingAxisTest(lhs.getSeparationAxes(), {}, {}, {}, lhs, rhs);
    }





    constexpr bool isIntersecting(const Sphere& lhs, const Sphere& rhs) noexcept {
        const float distanceSquared = (lhs.origin - rhs.origin).lengthSquared();
        const float combinedRadius = lhs.radius + rhs.radius;
        const float combinedRadiusSquared = combinedRadius * combinedRadius;

        return distanceSquared < combinedRadiusSquared;
    }

    constexpr bool isIntersecting(
        const SeparatingAxisShapeType3D auto& lhs,
        const SeparatingAxisShapeType3D auto& rhs
    ) noexcept {
        return isIntersecting(
            lhs.getSeparationAxes(),
            rhs.getSeparationAxes(),
            lhs.getSeparationEdges(),
            rhs.getSeparationEdges(),
            lhs,
            rhs
        );
    }

    constexpr bool isIntersecting(
        const std::span<const Vector3> axes,
        const ShapeType3D auto& lhs,
        const ShapeType3D auto& rhs
    ) noexcept {
        return std::ranges::all_of(
            axes,
            [&](const auto& axis) {
                const Projection3D p1 = lhs.project(axis);
                const Projection3D p2 = rhs.project(axis);
                return p1.getOverlap(p2) >= 0;
            }
        );
    }

    constexpr bool isIntersecting(
        const std::span<const Vector3> axes1,
        const std::span<const Vector3> axes2,
        const std::span<const Vector3> edges1,
        const std::span<const Vector3> edges2,
        const ShapeType3D auto& lhs,
        const ShapeType3D auto& rhs
    ) {
        const auto testAxis = [&](const auto& axis) {
            const Projection3D p1 = lhs.project(axis);
            const Projection3D p2 = rhs.project(axis);
            return p1.getOverlap(p2) >= 0;
        };

        for (const auto& axis : axes1)
            if (!testAxis(axis))
                return false;

        for (const auto& axis2 : axes2)
            if (!testAxis(axis2))
                return false;

        for (const auto& edge1 : edges1) {
            for (const auto& edge2 : edges2) {
                const Vector3 axis = edge1.cross(edge2);
                if (axis.lengthSquared() > 1e-6f) {
                    if (!testAxis(axis.normalise()))
                        return false;
                }
            }
        }

        return true;
    }

    constexpr bool isIntersecting(const Sphere& lhs, const SeparatingAxisShapeType3D auto& rhs) noexcept {
        const Vector3 closestPoint = rhs.getClosestPoint(lhs.origin);

        return closestPoint.distanceSquaredTo(lhs.origin) < lhs.radius * lhs.radius;
    }

    constexpr bool isIntersecting(const SeparatingAxisShapeType3D auto& lhs, const Sphere& rhs) noexcept {
        return isIntersecting(rhs, lhs);
    }

    constexpr bool isIntersecting(const AABB& lhs, const AABB& rhs) noexcept {
        return  std::abs(lhs.origin.x - rhs.origin.x) < (lhs.halfExtents.x + rhs.halfExtents.x) &&
                std::abs(lhs.origin.y - rhs.origin.y) < (lhs.halfExtents.y + rhs.halfExtents.y) &&
                std::abs(lhs.origin.z - rhs.origin.z) < (lhs.halfExtents.z + rhs.halfExtents.z);
    }
}
