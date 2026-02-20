#include <m3ds/nodes/3d/Area3D.hpp>

#include "m3ds/nodes/Viewport.hpp"

namespace M3DS {
    bool Area3D::isOverlapping(const Area3D* other) const noexcept {
        return mAccessor->isOverlapping(&*other->mAccessor);
    }

    void Area3D::afterTreeEnter() {
        CollisionObject3D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer3d().emplaceArea();
        mAccessor->userData = this;

        mAccessor->areaEntered = [this](const SPhys::Area3D* other) {
            areaEntered.emit(static_cast<Area3D*>(other->userData));
        };

        mAccessor->areaExited = [this](const SPhys::Area3D* other) {
            areaExited.emit(static_cast<Area3D*>(other->userData));
        };

        updateCollisionObject(mAccessor.get());
        externaliseState();
    }

    void Area3D::beforeTreeExit() {
        CollisionObject3D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer3d().eraseArea(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    Failure Area3D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (const Failure failure = areaEntered.serialise(this, serialiser))
            return failure;

        if (const Failure failure = areaExited.serialise(this, serialiser))
            return failure;

        return Success;
    }

    Failure Area3D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (const Failure failure = areaEntered.deserialise(this, deserialiser))
            return failure;

        if (const Failure failure = areaExited.deserialise(this, deserialiser))
            return failure;

        return Success;
    }

    REGISTER_METHODS(
        Area3D,

        CONST_METHOD(isOverlapping)
    );

    REGISTER_NO_MEMBERS(Area3D);
}
