#pragma once

#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/utils/Units.hpp>

namespace M3DS {
    struct Transform2D {
        Pixels<Vector2> position {};
        Vector2 scale { 1 };
        Radians<float> rotation {};

        [[nodiscard]] constexpr Transform2D offset(
            const Pixels<Vector2>& newPosition,
            const Vector2& newScale = Vector2(1),
            Radians<float> newRotation = 0
        ) const;

        [[nodiscard]] constexpr Transform2D offset(const Transform2D& other) const;

        [[nodiscard]] constexpr Transform2D inverse() const;
    };

    constexpr Transform2D Transform2D::offset(
        const Pixels<Vector2>& newPosition,
        const Vector2& newScale,
        const float newRotation
    ) const {
        Transform2D result = *this;

        result.position += (newPosition * scale).rotated(rotation);
        result.scale *= newScale;
        result.rotation += newRotation;

        return result;
    }

    constexpr Transform2D Transform2D::offset(const Transform2D& other) const {
        return offset(other.position, other.scale, other.rotation);
    }

    constexpr Transform2D Transform2D::inverse() const {
        return Transform2D(
            -(this->position / scale).rotated(-rotation),
            Vector2(1) / scale,
            -rotation
        );
    }

    constexpr std::ostream& operator<<(std::ostream& stream, const Transform2D& xform) {
        return stream << "[" << xform.position << ", " << xform.scale << ", " << xform.rotation << "]";
    }
}
