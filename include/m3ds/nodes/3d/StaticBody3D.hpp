#pragma once

#include <m3ds/nodes/3d/PhysicsBody3D.hpp>

namespace M3DS {
    class StaticBody3D : public PhysicsBody3D {
        M_CLASS(StaticBody3D, PhysicsBody3D)
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        SPhys::Accessor<SPhys::StaticBody3D> mAccessor {};
    };
}
