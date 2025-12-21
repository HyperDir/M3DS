#pragma once

#include <m3ds/nodes/2d/CollisionObject2D.hpp>
#include <m3ds/types/Signal.hpp>

#include <m3ds/lib/SPhys/2D/CollisionObjects/Area2D.hpp>
#include <m3ds/lib/SPhys/Utils/Accessor.hpp>

namespace M3DS {
    class Area2D : public CollisionObject2D {
        M_CLASS(Area2D, CollisionObject2D)
    public:
        AbstractSignal<Area2D*> areaEntered {};
        AbstractSignal<Area2D*> areaExited {};

        [[nodiscard]] bool isOverlapping(const Area2D* other) const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        SPhys::Accessor<SPhys::Area2D> mAccessor {};
    };
}
