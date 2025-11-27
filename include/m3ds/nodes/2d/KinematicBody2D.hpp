#pragma once

#include <m3ds/nodes/2d/PhysicsBody2D.hpp>

#include <m3ds/lib/SPhys/2D/CollisionObjects/KinematicBody2D.hpp>

namespace M3DS {
    class KinematicBody2D : public PhysicsBody2D {
        M_CLASS(KinematicBody2D, PhysicsBody2D)
    public:
        explicit KinematicBody2D();

        void setVelocity(const PixelsPerSecond<Vector2>& to) noexcept;
        void setUpDirection(const Vector2& to) noexcept;
        void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] const PixelsPerSecond<Vector2>& getVelocity() const noexcept;
        [[nodiscard]] const Vector2& getUpDirection() const noexcept;
        [[nodiscard]] bool getSlideOnSlope() const noexcept;

        [[nodiscard]] bool isOnGround() const noexcept;
    private:
        SPhys::KinematicBody2D mSpecialisedObject {};
    };
}
