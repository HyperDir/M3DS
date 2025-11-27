#pragma once

#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector2.hpp"

#include "Projection2D.hpp"

namespace SPhys {
    struct Rect2D {
        Pixels<Vector2> origin {};
        Radians<float> rotation {}; // Maybe rotation should cache sin and cos? can have overloaded operator=(float)
        Pixels<Vector2> halfExtents {};

        constexpr void setTranslation(const Vector2& to) noexcept;
        [[nodiscard]] constexpr const Vector2& getTranslation() const noexcept;

        constexpr void rotate(float by) noexcept;

        constexpr void setRotation(float to) noexcept;
        [[nodiscard]] constexpr float getRotation() const noexcept;

        [[nodiscard]] constexpr bool contains(const Vector2& point) const noexcept;
        [[nodiscard]] constexpr Vector2 getSurfaceNormal(const Vector2& point) const noexcept;
        
        [[nodiscard]] constexpr Projection2D project(const Vector2& axis) const noexcept;
        [[nodiscard]] constexpr std::array<Vector2, 2> getSeparationAxes() const noexcept;
        [[nodiscard]] constexpr Vector2 getClosestPoint(const Vector2& to) const noexcept;

        [[nodiscard]] constexpr BoundingBox2D getBoundingBox() const noexcept;
    };

    constexpr void Rect2D::setTranslation(const Vector2& to) noexcept {
        origin = to;
    }

    constexpr const Vector2& Rect2D::getTranslation() const noexcept {
        return origin;
    }

    constexpr void Rect2D::rotate(const float by) noexcept {
        rotation += by;
    }

    constexpr void Rect2D::setRotation(const float to) noexcept {
        rotation = to;
    }

    constexpr float Rect2D::getRotation() const noexcept {
        return rotation;
    }

    constexpr bool Rect2D::contains(const Vector2& point) const noexcept {
        const Vector2 localPoint = (point - origin).rotated(-rotation);

        return (
            std::abs(localPoint.x) <= halfExtents.x &&
            std::abs(localPoint.y) <= halfExtents.y
        );
    }

    constexpr Vector2 Rect2D::getSurfaceNormal(const Vector2& point) const noexcept {
        const Vector2 localPoint = (point - origin).rotated(-rotation);

        const Vector2 n {
            std::abs(localPoint.x * halfExtents.y),
            std::abs(localPoint.y * halfExtents.x),
        };

        if (n.x > n.y) {
            return Vector2{ (localPoint.x > 0 ? 1.f : -1.f), 0 }.rotated(rotation);
        }
        return Vector2{ 0, (localPoint.y > 0 ? 1.f : -1.f) }.rotated(rotation);
    }

    constexpr Projection2D Rect2D::project(const Vector2& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);

        const float sinR = std::sin(rotation);
        const float cosR = std::cos(rotation);

        const Vector2 xAxis { cosR, sinR };
        const Vector2 yAxis { -sinR, cosR };

        const float radius = std::abs(xAxis.dot(axis) * halfExtents.x) + std::abs(yAxis.dot(axis) * halfExtents.y);

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr std::array<Vector2, 2> Rect2D::getSeparationAxes() const noexcept {
        const float sinR = std::sin(rotation);
        const float cosR = std::cos(rotation);
        return {
            Vector2{ cosR, sinR },
            Vector2{ -sinR, cosR }
        };
    }

    constexpr Vector2 Rect2D::getClosestPoint(const Vector2& to) const noexcept {
        const float sinR = std::sin(rotation);
        const float cosR = std::cos(rotation);
        return origin + (to - origin).rotated(-sinR, cosR).clamp(-halfExtents, halfExtents).rotated(sinR, cosR);
    }

    constexpr BoundingBox2D Rect2D::getBoundingBox() const noexcept {
        const Vector2 u = Vector2{1, 0}.rotated(rotation);
        const Vector2 v = Vector2{0, 1}.rotated(rotation);

        const Vector2 alignedExtents = {
            std::abs(u.x) * halfExtents.x + std::abs(v.x) * halfExtents.y,
            std::abs(u.y) * halfExtents.x + std::abs(v.y) * halfExtents.y
        };

        return { origin - alignedExtents, origin + alignedExtents };
    }
}
