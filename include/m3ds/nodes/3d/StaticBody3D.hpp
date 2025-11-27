#pragma once

#include <m3ds/nodes/3d/PhysicsBody3D.hpp>

#include <m3ds/lib/SPhys/3D/CollisionObjects/StaticBody3D.hpp>

namespace M3DS {
    class StaticBody3D : public PhysicsBody3D {
        M_CLASS(StaticBody3D, PhysicsBody3D)
    public:
        StaticBody3D();
    private:
        SPhys::StaticBody3D mSpecialisedObject {};
    };
}
