#pragma once

#include <m3ds/nodes/2d/PhysicsBody2D.hpp>

#include <m3ds/lib/SPhys/2D/CollisionObjects/StaticBody2D.hpp>

namespace M3DS {
    class StaticBody2D : public PhysicsBody2D {
        M_CLASS(StaticBody2D, PhysicsBody2D)
    public:
        StaticBody2D();
    private:
        SPhys::StaticBody2D mSpecialisedObject {};
    };
}
