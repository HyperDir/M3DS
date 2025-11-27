#pragma once

#include "PhysicsBody2D.hpp"

namespace SPhys {
    class KinematicBody2D : public PhysicsBody2D {
        template <PhysicsEnvironment2D> friend class PhysicsServer2D;
    public:
        explicit constexpr KinematicBody2D(
            const Pixels<Vector2>& translation = {},
            float rotation = {},
            const Shape2D& shape = {},
            const Vector2& upDirection = {0, 1},
            bool slideOnSlope = false
        ) noexcept;

        constexpr void setVelocity(const PixelsPerSecond<Vector2>& to) noexcept;
        constexpr void addVelocity(const PixelsPerSecond<Vector2>& amount) noexcept;
        constexpr void subtractVelocity(const PixelsPerSecond<Vector2>& amount) noexcept;

        constexpr void setUpDirection(const Vector2& to) noexcept;
        constexpr void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] constexpr const PixelsPerSecond<Vector2>& getVelocity() const noexcept;
        [[nodiscard]] constexpr const Vector2& getUpDirection() const noexcept;
        [[nodiscard]] constexpr bool getSlideOnSlope() const noexcept;
        [[nodiscard]] constexpr bool isOnGround() const noexcept;
    private:
        PixelsPerSecond<Vector2> mVelocity {};
        Vector2 mUpDirection { 0, 1};
        bool mSlideOnSlope {};
        bool mOnGround {};
    };

    constexpr KinematicBody2D::KinematicBody2D(
        const Pixels<Vector2>& translation,
        float rotation,
        const Shape2D& shape,
        const Vector2& upDirection,
        const bool slideOnSlope
    ) noexcept
        : PhysicsBody2D(translation, rotation, shape)
        , mUpDirection(upDirection)
        , mSlideOnSlope(slideOnSlope)
    {
        mObjectType = ObjectType2D::kinematic_body;
    }

    constexpr void KinematicBody2D::setVelocity(const PixelsPerSecond<Vector2>& to) noexcept {
        mVelocity = to;
    }

    constexpr void KinematicBody2D::addVelocity(const PixelsPerSecond<Vector2>& amount) noexcept {
        mVelocity += amount;
    }

    constexpr void KinematicBody2D::subtractVelocity(const PixelsPerSecond<Vector2>& amount) noexcept {
        mVelocity -= amount;
    }

    constexpr void KinematicBody2D::setUpDirection(const Vector2& to) noexcept {
        mUpDirection = to;
    }

    constexpr void KinematicBody2D::setSlideOnSlope(const bool to) noexcept {
        mSlideOnSlope = to;
    }

    constexpr const PixelsPerSecond<Vector2>& KinematicBody2D::getVelocity() const noexcept {
        return mVelocity;
    }

    constexpr const Vector2& KinematicBody2D::getUpDirection() const noexcept {
        return mUpDirection;
    }

    constexpr bool KinematicBody2D::getSlideOnSlope() const noexcept {
        return mSlideOnSlope;
    }

    constexpr bool KinematicBody2D::isOnGround() const noexcept {
        return mOnGround;
    }
}
