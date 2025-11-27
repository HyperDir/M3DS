#pragma once

#include <m3ds/nodes/3d/PhysicsBody3D.hpp>
#include <m3ds/lib/SPhys/3D/CollisionObjects/KinematicBody3D.hpp>

namespace M3DS {
    class KinematicBody3D : public PhysicsBody3D {
        M_CLASS(KinematicBody3D, PhysicsBody3D)
    public:
        explicit KinematicBody3D();

        void setVelocity(const MetresPerSecond<Vector3>& to) noexcept;
        void setUpDirection(const Vector3& to) noexcept;
        void setSlideOnSlope(bool to) noexcept;

        [[nodiscard]] const MetresPerSecond<Vector3>& getVelocity() const noexcept;
        [[nodiscard]] const Vector3& getUpDirection() const noexcept;
        [[nodiscard]] bool getSlideOnSlope() const noexcept;

        [[nodiscard]] bool isOnGround() const noexcept;
    private:
        SPhys::KinematicBody3D mSpecialisedObject {};
    };
}
