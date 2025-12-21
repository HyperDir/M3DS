#pragma once

#include <m3ds/nodes/3d/CollisionObject3D.hpp>
#include <m3ds/types/Signal.hpp>

#include <m3ds/lib/SPhys/3D/CollisionObjects/Area3D.hpp>
#include <m3ds/lib/SPhys/Utils/Accessor.hpp>

namespace M3DS {
    class Area3D : public CollisionObject3D {
        M_CLASS(Area3D, CollisionObject3D)
    public:
        AbstractSignal<Area3D*> areaEntered {};
        AbstractSignal<Area3D*> areaExited {};

        [[nodiscard]] bool isOverlapping(const Area3D* other) const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        SPhys::Accessor<SPhys::Area3D> mAccessor {};
    };
}
