#pragma once

#include <m3ds/nodes/3d/CollisionObject3D.hpp>

namespace M3DS {
    class PhysicsBody3D : public CollisionObject3D {
        M_CLASS(PhysicsBody3D, CollisionObject3D)
    protected:
        explicit PhysicsBody3D(SPhys::CollisionObject3D* collisionObject);
    };
}
