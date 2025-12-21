#pragma once

#include <m3ds/nodes/3d/PhysicsBody3D.hpp>

#include <m3ds/lib/SPhys/3D/CollisionObjects/KinematicBody3D.hpp>

namespace M3DS {
    class KinematicBody3D : public PhysicsBody3D {
        M_CLASS(KinematicBody3D, PhysicsBody3D)
    public:
        void setVelocity(const MetresPerSecond<Vector3>& to) noexcept;
        void setUpDirection(const Vector3& to) noexcept;
        void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] const MetresPerSecond<Vector3>& getVelocity() const noexcept;
        [[nodiscard]] const Vector3& getUpDirection() const noexcept;
        [[nodiscard]] bool getSlideOnSlope() const noexcept;

        [[nodiscard]] bool isOnGround() const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;

        void internaliseState() noexcept override;
        void externaliseState() noexcept override;
    private:
        SPhys::Accessor<SPhys::KinematicBody3D> mAccessor {};

        struct InternalData {
            Vector3 velocity {};
            Vector3 upDirection {0.f, 1.f, 0.f};
            bool slideOnSlope {};
        } mSerialisationData {};
    };
}
