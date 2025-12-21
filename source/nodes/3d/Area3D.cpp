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
            areaEntered.emit(this, static_cast<Area3D*>(other->userData));
        };

        mAccessor->areaExited = [this](const SPhys::Area3D* other) {
            areaExited.emit(this, static_cast<Area3D*>(other->userData));
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

    Error Area3D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = CollisionObject3D::serialise(file); error != Error::none)
            return error;

        if (const Error error = areaEntered.serialise(file); error != Error::none)
            return error;

        if (const Error error = areaExited.serialise(file); error != Error::none)
            return error;

        return Error::none;
    }

    Error Area3D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = CollisionObject3D::deserialise(file); error != Error::none)
            return error;

        if (const Error error = areaEntered.deserialise(file); error != Error::none)
            return error;

        if (const Error error = areaExited.deserialise(file); error != Error::none)
            return error;

        return Error::none;
    }

    REGISTER_METHODS(
        Area3D,

        CONST_METHOD(isOverlapping)
    );

    REGISTER_NO_MEMBERS(Area3D);
}
