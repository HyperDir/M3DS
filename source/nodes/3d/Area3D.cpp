#include <m3ds/nodes/3d/Area3D.hpp>

namespace M3DS {
    Area3D::Area3D()
        : CollisionObject3D(&mSpecialisedObject)
    {
        mSpecialisedObject.areaEntered = [this](const SPhys::Area3D* area) {
            areaEntered.emit(this, static_cast<Area3D*>(area->userData));
        };
        mSpecialisedObject.areaExited = [this](const SPhys::Area3D* area) {
            areaExited.emit(this, static_cast<Area3D*>(area->userData));
        };

        mSpecialisedObject.userData = this;
    }

    bool Area3D::isOverlapping(const Area3D* other) const noexcept {
        return mSpecialisedObject.isOverlapping(&other->mSpecialisedObject);
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
