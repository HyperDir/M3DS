#pragma once

#include <m3ds/nodes/3d/CollisionObject3D.hpp>
#include <m3ds/types/Signal.hpp>
#include <m3ds/lib/SPhys/3D/CollisionObjects/Area3D.hpp>

namespace M3DS {
    class Area3D : public CollisionObject3D {
        M_CLASS(Area3D, CollisionObject3D)
    public:
        Area3D();

        AbstractSignal<Area3D*> areaEntered {};
        AbstractSignal<Area3D*> areaExited {};

        [[nodiscard]] bool isOverlapping(const Area3D* other) const noexcept;
    private:
        SPhys::Area3D mSpecialisedObject {};
    };
}
