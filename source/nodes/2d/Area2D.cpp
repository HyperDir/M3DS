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
            areaEntered.emit(this, static_cast<Area2D*>(other->userData));
        };

        mAccessor->areaExited = [this](const SPhys::Area2D* other) {
            areaExited.emit(this, static_cast<Area2D*>(other->userData));
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

    Error Area2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = CollisionObject2D::serialise(file); error != Error::none)
            return error;

        if (const Error error = areaEntered.serialise(file); error != Error::none)
            return error;

        if (const Error error = areaExited.serialise(file); error != Error::none)
            return error;

        return Error::none;
    }

    Error Area2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = CollisionObject2D::deserialise(file); error != Error::none)
            return error;

        if (const Error error = areaEntered.deserialise(file); error != Error::none)
            return error;

        if (const Error error = areaExited.deserialise(file); error != Error::none)
            return error;

        return Error::none;
    }

    REGISTER_METHODS(
        Area2D,

        CONST_METHOD(isOverlapping)
    );

    REGISTER_NO_MEMBERS(Area2D);
}
