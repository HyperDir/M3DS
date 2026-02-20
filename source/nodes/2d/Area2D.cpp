#include <m3ds/nodes/2d/Area2D.hpp>

#include "m3ds/nodes/Viewport.hpp"

namespace M3DS {
    bool Area2D::isOverlapping(const Area2D* other) const noexcept {
        return mAccessor->isOverlapping(&*other->mAccessor);
    }

    void Area2D::afterTreeEnter() {
        CollisionObject2D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer2d().emplaceArea();
        mAccessor->userData = this;

        mAccessor->areaEntered = [this](const SPhys::Area2D* other) {
            areaEntered.emit(static_cast<Area2D*>(other->userData));
        };

        mAccessor->areaExited = [this](const SPhys::Area2D* other) {
            areaExited.emit(static_cast<Area2D*>(other->userData));
        };

        updateCollisionObject(mAccessor.get());
        externaliseState();
    }

    void Area2D::beforeTreeExit() {
        CollisionObject2D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer2d().eraseArea(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    Failure Area2D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (const Failure failure = areaEntered.serialise(this, serialiser))
            return failure;

        if (const Failure failure = areaExited.serialise(this, serialiser))
            return failure;

        return Success;
    }

    Failure Area2D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (const Failure failure = areaEntered.deserialise(this, deserialiser))
            return failure;

        if (const Failure failure = areaExited.deserialise(this, deserialiser))
            return failure;

        return Success;
    }

    REGISTER_METHODS(
        Area2D,

        CONST_METHOD(isOverlapping)
    );

    REGISTER_NO_MEMBERS(Area2D);
}
