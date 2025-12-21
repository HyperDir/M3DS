#pragma once

#include <m3ds/nodes/2d/PhysicsBody2D.hpp>

#include <m3ds/lib/SPhys/2D/CollisionObjects/KinematicBody2D.hpp>

namespace M3DS {
    class KinematicBody2D : public PhysicsBody2D {
        M_CLASS(KinematicBody2D, PhysicsBody2D)
    public:
        void setVelocity(const PixelsPerSecond<Vector2>& to) noexcept;
        void setUpDirection(const Vector2& to) noexcept;
        void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] const PixelsPerSecond<Vector2>& getVelocity() const noexcept;
        [[nodiscard]] const Vector2& getUpDirection() const noexcept;
        [[nodiscard]] bool getSlideOnSlope() const noexcept;

        [[nodiscard]] bool isOnGround() const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;

        void internaliseState() noexcept override;
        void externaliseState() noexcept override;
    private:
        SPhys::Accessor<SPhys::KinematicBody2D> mAccessor {};

        struct InternalData {
            Vector2 velocity {};
            Vector2 upDirection {0.f, 1.f};
            bool slideOnSlope {};
        } mSerialisationData {};
    };
}
