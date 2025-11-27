#pragma once

#include <algorithm>

#include "Shape2D.hpp"

namespace SPhys {
    // Minimum Translation Vector (shortest move distance required to resolve collision)
    struct MTV2D {
        Vector2 normal {};
        float magnitude {};

        [[nodiscard]] constexpr Vector2 get() const noexcept;
    };

    constexpr std::optional<MTV2D> separatingAxisTest(
        const SeparatingAxisShapeType2D auto& lhs,
        const SeparatingAxisShapeType2D auto& rhs
    ) noexcept;

    constexpr bool isIntersecting(
        const SeparatingAxisShapeType2D auto& lhs,
        const SeparatingAxisShapeType2D auto& rhs
    ) noexcept;
}

/* Implementation */
namespace SPhys {
    constexpr Vector2 MTV2D::get() const noexcept {
        return normal * magnitude;
    }

    constexpr std::optional<MTV2D> separatingAxisTest(const Circle2D& lhs, const Circle2D& rhs) noexcept {
        const Vector2 delta = lhs.origin - rhs.origin;
        const float distanceSquared = delta.lengthSquared();
        const float combinedRadius = lhs.radius + rhs.radius;

        if (distanceSquared >= combinedRadius * combinedRadius) return {};

        // If origins overlap, push out along X-axis
        if (std::abs(distanceSquared) < 1e-6f) {
            return { MTV2D{{&lhs > &rhs ? 1.f : -1.f, 0}, combinedRadius} };
        }

        const float distance = std::sqrt(distanceSquared);
        const float overlap = combinedRadius - distance;

        const Vector2 collisionNormal = delta / distance;
        return { MTV2D{collisionNormal, overlap} };
    }

    constexpr std::optional<MTV2D> separatingAxisTest(
        const SeparatingAxisShapeType2D auto& lhs,
        const SeparatingAxisShapeType2D auto& rhs
    ) noexcept {
        return separatingAxisTest(
            lhs.getSeparationAxes(),
            rhs.getSeparationAxes(),
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV2D> separatingAxisTest(
        const std::span<const Vector2> axes1,
        const std::span<const Vector2> axes2,
        const ShapeType2D auto& lhs,
        const ShapeType2D auto& rhs
    ) noexcept {
        // https://dyn4j.org/3010/01/sat/
        float minOverlap = 1e10f;
        const Vector2* smallestAxis {};

        bool negate = false;

        #define AXIS_TEST_2D(axis) {                            \
            const Projection2D p1 = lhs.project(axis);          \
            const Projection2D p2 = rhs.project(axis);          \
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
            AXIS_TEST_2D(axis);

        for (const auto& axis : axes2)
            AXIS_TEST_2D(axis);

        if (!smallestAxis) return {};

        return { MTV2D{ negate ? -*smallestAxis : *smallestAxis, minOverlap } };
    }

    constexpr std::optional<MTV2D> separatingAxisTest(const Circle2D& lhs, const SeparatingAxisShapeType2D auto& rhs) noexcept {
        const Vector2 closestPoint = rhs.getClosestPoint(lhs.origin);

        if (closestPoint.distanceSquaredTo(lhs.origin) < 1e-6f) {
            return separatingAxisTest(
                rhs.getSeparationAxes(),
                {},
                lhs,
                rhs
            );
        }

        return separatingAxisTest(
            {(lhs.origin - closestPoint).normalise()},
            rhs.getSeparationAxes(),
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV2D> separatingAxisTest(const SeparatingAxisShapeType2D auto& lhs, const Circle2D& rhs) noexcept {
        const Vector2 closestPoint = lhs.getClosestPoint(rhs.origin);

        if (closestPoint.distanceSquaredTo(rhs.origin) < 1e-6f) {
            return separatingAxisTest(
                lhs.getSeparationAxes(),
                {},
                lhs,
                rhs
            );
        }

        return separatingAxisTest(
            lhs.getSeparationAxes(),
            {(rhs.origin - closestPoint).normalise()},
            lhs,
            rhs
        );
    }

    constexpr std::optional<MTV2D> separatingAxisTest(const AARect2D& lhs, const AARect2D& rhs) noexcept {
        return separatingAxisTest(lhs.getSeparationAxes(), {}, lhs, rhs);
    }





    constexpr bool isIntersecting(const Circle2D& lhs, const Circle2D& rhs) noexcept {
        const float distanceSquared = (lhs.origin - rhs.origin).lengthSquared();
        const float combinedRadius = lhs.radius + rhs.radius;
        const float combinedRadiusSquared = combinedRadius * combinedRadius;

        return distanceSquared < combinedRadiusSquared;
    }

    constexpr bool isIntersecting(
        const SeparatingAxisShapeType2D auto& lhs,
        const SeparatingAxisShapeType2D auto& rhs
    ) noexcept {
        return isIntersecting(
            lhs.getSeparationAxes(),
            rhs.getSeparationAxes(),
            lhs,
            rhs
        );
    }

    constexpr bool isIntersecting(
        const std::span<const Vector2> axes,
        const ShapeType2D auto& lhs,
        const ShapeType2D auto& rhs
    ) noexcept {
        return std::ranges::all_of(
            axes,
            [&](const auto& axis) {
                const Projection2D p1 = lhs.project(axis);
                const Projection2D p2 = rhs.project(axis);
                return p1.getOverlap(p2) >= 0;
            }
        );
    }

    constexpr bool isIntersecting(
        const std::span<const Vector2> axes1,
        const std::span<const Vector2> axes2,
        const ShapeType2D auto& lhs,
        const ShapeType2D auto& rhs
    ) {
        const auto testAxis = [&](const auto& axis) {
            const Projection2D p1 = lhs.project(axis);
            const Projection2D p2 = rhs.project(axis);
            return p1.getOverlap(p2) >= 0;
        };

        for (const auto& axis : axes1)
            if (!testAxis(axis))
                return false;

        for (const auto& axis2 : axes2)
            if (!testAxis(axis2))
                return false;

        return true;
    }

    constexpr bool isIntersecting(const Circle2D& lhs, const SeparatingAxisShapeType2D auto& rhs) noexcept {
        const Vector2 closestPoint = rhs.getClosestPoint(lhs.origin);

        return closestPoint.distanceSquaredTo(lhs.origin) < lhs.radius * lhs.radius;
    }

    constexpr bool isIntersecting(const SeparatingAxisShapeType2D auto& lhs, const Circle2D& rhs) noexcept {
        return isIntersecting(rhs, lhs);
    }

    constexpr bool isIntersecting(const AARect2D& lhs, const AARect2D& rhs) noexcept {
        return  std::abs(lhs.origin.x - rhs.origin.x) < (lhs.halfExtents.x + rhs.halfExtents.x) &&
                std::abs(lhs.origin.y - rhs.origin.y) < (lhs.halfExtents.y + rhs.halfExtents.y);
    }
}

// #pragma once
//
// #include "Shape2D.hpp"
//
// #include "Shapes/BoundingBox2D.hpp"
//
// #include <algorithm>
//
// namespace SPhys {
//     // Minimum Translation Vector (shortest move distance required to resolve collision)
//     struct MTV2D {
//         Vector2 normal {};
//         float magnitude {};
//
//         [[nodiscard]] constexpr Vector2 get() const noexcept;
//     };
//
//     constexpr BoundingBox2D getBoundingBox(const Circle2D& circle) noexcept;
//     constexpr BoundingBox2D getBoundingBox(const Rect2D& rect2d) noexcept;
//     constexpr BoundingBox2D getBoundingBox(const Triangle2D& triangle2d) noexcept;
//
//     constexpr std::optional<MTV2D> isIntersecting(const Circle2D& lhs, const Circle2D& rhs) noexcept;
//     constexpr std::optional<MTV2D> isIntersecting(const Circle2D& lhs, const SeparatingAxisShapeType2D auto& rhs) noexcept;
//     constexpr std::optional<MTV2D> isIntersecting(const SeparatingAxisShapeType2D auto& lhs, const Circle2D& rhs) noexcept;
//
//     constexpr std::optional<MTV2D> isIntersecting(
//         const SeparatingAxisShapeType2D auto& lhs, 
//         const SeparatingAxisShapeType2D auto& rhs
//     ) noexcept;
//
//     constexpr std::optional<MTV2D> isIntersecting(
//         std::span<const Vector2> axes1,
//         std::span<const Vector2> axes2,
//         const ShapeType2D auto& lhs,
//         const ShapeType2D auto& rhs
//     ) noexcept;
// }
//
// namespace SPhys {
//     constexpr BoundingBox2D getBoundingBox(const Circle2D& circle) noexcept {
//         return { circle.origin, { circle.radius, circle.radius } };
//     }
//
//     constexpr BoundingBox2D getBoundingBox(const Rect2D& rect2d) noexcept {
//         const float cosR = std::abs(std::cos(rect2d.rotation));
//         const float sinR = std::abs(std::sin(rect2d.rotation));
//         return { 
//             rect2d.origin,
//             {
//                 cosR * rect2d.halfExtents.x + sinR * rect2d.halfExtents.y,
//                 sinR * rect2d.halfExtents.x + cosR * rect2d.halfExtents.y
//             }
//         };
//     }
//
//     constexpr BoundingBox2D getBoundingBox(const Triangle2D& triangle2d) noexcept {
//         const Vector2 min {
//             std::min({triangle2d.vertices[0].x, triangle2d.vertices[1].x, triangle2d.vertices[2].x}),
//             std::min({triangle2d.vertices[0].y, triangle2d.vertices[1].y, triangle2d.vertices[2].y})
//         };
//         const Vector2 max {
//             std::max({triangle2d.vertices[0].x, triangle2d.vertices[1].x, triangle2d.vertices[2].x}),
//             std::max({triangle2d.vertices[0].y, triangle2d.vertices[1].y, triangle2d.vertices[2].y})
//         };
//
//         return {
//             (min + max) / 2.f,
//             (max - min) / 2.f
//         };
//     }
//
//     constexpr Vector2 MTV2D::get() const noexcept {
//         return normal * magnitude;
//     }
//
//     constexpr std::optional<MTV2D> isIntersecting(const Circle2D& lhs, const Circle2D& rhs) noexcept {
//         const Vector2 delta = rhs.origin - lhs.origin;
//         const float distanceSquared = delta.lengthSquared();
//         const float combinedRadius = lhs.radius + rhs.radius;
//         const float combinedRadiusSquared = combinedRadius * combinedRadius;
//
//         if (distanceSquared >= combinedRadiusSquared) return {};
//
//         // If origins overlap, push out along X-axis
//         if (std::abs(distanceSquared) < 1e-6f) {
//             return { MTV2D{{1, 0}, combinedRadius} };
//         }
//
//         const float distance = std::sqrt(distanceSquared);
//         const float overlap = combinedRadius - distance;
//
//         const Vector2 collisionNormal = delta / distance;
//         return { MTV2D{collisionNormal, overlap} };
//     }
//
//     constexpr std::optional<MTV2D> isIntersecting(
//         const SeparatingAxisShapeType2D auto& lhs, 
//         const SeparatingAxisShapeType2D auto& rhs
//     ) noexcept {
//         return isIntersecting(lhs.getSeparationAxes(), rhs.getSeparationAxes(), lhs, rhs);
//     }
//
//     constexpr std::optional<MTV2D> isIntersecting(
//         std::span<const Vector2> axes1,
//         std::span<const Vector2> axes2,
//         const ShapeType2D auto& lhs,
//         const ShapeType2D auto& rhs
//     ) noexcept {
//         // https://dyn4j.org/2010/01/sat/
//         float minOverlap = 1e10f;
//         const Vector2* smallestAxis {};
//
//         bool negate = false;
//
//         for (const auto& axis : axes1) {
//             const Projection2D p1 = lhs.project(axis);
//             const Projection2D p2 = rhs.project(axis);
//             float overlap = p1.getOverlap(p2);
//             if (overlap <= 0)
//                 return {};
//             
//             if (p1.contains(p2) || p2.contains(p1)) {
//                 const float mins = std::abs(p1.from - p2.from);
//                 const float maxes = std::abs(p1.to - p2.to);
//
//                 if (mins < maxes) overlap += mins;
//                 else overlap += maxes;
//             }
//
//             if (overlap < minOverlap) {
//                 minOverlap = overlap;
//                 smallestAxis = &axis;
//
//                 negate = p2.from + p2.to > p1.from + p1.to;
//             }
//         }
//
//         for (const auto& axis : axes2) {
//             const Projection2D p1 = lhs.project(axis);
//             const Projection2D p2 = rhs.project(axis);
//             float overlap = p1.getOverlap(p2);
//             if (overlap <= 0)
//                 return {};
//             
//             if (p1.contains(p2) || p2.contains(p1)) {
//                 const float mins = std::abs(p1.from - p2.from);
//                 const float maxes = std::abs(p1.to - p2.to);
//
//                 if (mins < maxes) overlap += mins;
//                 else overlap += maxes;
//             }
//
//             if (overlap < minOverlap) {
//                 minOverlap = overlap;
//                 smallestAxis = &axis;
//
//                 negate = p2.from + p2.to > p1.from + p1.to;
//             }
//         }
//
//         if (!smallestAxis) return {};
//
//         return { MTV2D{ negate ? -*smallestAxis : *smallestAxis, minOverlap } };
//     }
//
//     constexpr std::optional<MTV2D> isIntersecting(const Circle2D& lhs, const SeparatingAxisShapeType2D auto& rhs) noexcept {
//         const Vector2 closestPoint = rhs.getClosestPoint(lhs.origin);
//
//         // Maybe precision issues
//         if (closestPoint == lhs.origin) {
//             return isIntersecting(
//                 std::span<Vector2>{},
//                 rhs.getSeparationAxes(),
//                 lhs,
//                 rhs
//             );
//         }
//
//         return isIntersecting(
//             std::array{(closestPoint - lhs.origin).normalise()}, 
//             rhs.getSeparationAxes(), 
//             lhs, 
//             rhs
//         );
//     }
//
//     constexpr std::optional<MTV2D> isIntersecting(const SeparatingAxisShapeType2D auto& lhs, const Circle2D& rhs) noexcept {
//         const Vector2 closestPoint = lhs.getClosestPoint(rhs.origin);
//
//         // Maybe precision issues
//         if (closestPoint == rhs.origin) {
//             return isIntersecting(
//                 lhs.getSeparationAxes(),
//                 std::span<Vector2>{},
//                 lhs,
//                 rhs
//             );
//         }
//
//         return isIntersecting(
//             lhs.getSeparationAxes(),
//             std::array{(closestPoint - rhs.origin).normalise()},
//             lhs,
//             rhs
//         );
//     }
// }