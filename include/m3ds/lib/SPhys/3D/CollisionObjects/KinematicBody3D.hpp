#pragma once

#include "PhysicsBody3D.hpp"

namespace SPhys {
    class KinematicBody3D : public PhysicsBody3D {
        template <PhysicsEnvironment3D> friend class PhysicsServer3D;
    public:
        constexpr KinematicBody3D() noexcept;

        constexpr void setVelocity(const MetresPerSecond<Vector3>& to) noexcept;
        constexpr void addVelocity(const MetresPerSecond<Vector3>& amount) noexcept;
        constexpr void subtractVelocity(const MetresPerSecond<Vector3>& amount) noexcept;

        constexpr void setUpDirection(const Vector3& to) noexcept;
        constexpr void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] constexpr const MetresPerSecond<Vector3>& getVelocity() const noexcept;
        [[nodiscard]] constexpr const Vector3& getUpDirection() const noexcept;
        [[nodiscard]] constexpr bool getSlideOnSlope() const noexcept;
        [[nodiscard]] constexpr bool isOnGround() const noexcept;
    private:
        MetresPerSecond<Vector3> mVelocity {};
        Vector3 mUpDirection { 0.f, 1.f, 0.f };
        bool mSlideOnSlope {};
        bool mOnGround {};
    };

    constexpr KinematicBody3D::KinematicBody3D() noexcept : PhysicsBody3D(ObjectType3D::kinematic_body) {}

    constexpr void KinematicBody3D::setVelocity(const MetresPerSecond<Vector3>& to) noexcept {
        mVelocity = to;
    }

    constexpr void KinematicBody3D::addVelocity(const MetresPerSecond<Vector3>& amount) noexcept {
        mVelocity += amount;
    }

    constexpr void KinematicBody3D::subtractVelocity(const MetresPerSecond<Vector3>& amount) noexcept {
        mVelocity -= amount;
    }

    constexpr void KinematicBody3D::setUpDirection(const Vector3& to) noexcept {
        mUpDirection = to;
    }

    constexpr void KinematicBody3D::setSlideOnSlope(const bool to) noexcept {
        mSlideOnSlope = to;
    }

    constexpr const MetresPerSecond<Vector3>& KinematicBody3D::getVelocity() const noexcept {
        return mVelocity;
    }

    constexpr const Vector3& KinematicBody3D::getUpDirection() const noexcept {
        return mUpDirection;
    }

    constexpr bool KinematicBody3D::getSlideOnSlope() const noexcept {
        return mSlideOnSlope;
    }

    constexpr bool KinematicBody3D::isOnGround() const noexcept {
        return mOnGround;
    }
}
