#include <m3ds/nodes/2d/Area2D.hpp>

namespace M3DS {
    Area2D::Area2D()
        : CollisionObject2D(&mSpecialisedObject)
    {
        mSpecialisedObject.areaEntered = [this](const SPhys::Area2D* area) {
            areaEntered.emit(this, static_cast<Area2D*>(area->userData));
        };
        mSpecialisedObject.areaExited = [this](const SPhys::Area2D* area) {
            areaExited.emit(this, static_cast<Area2D*>(area->userData));
        };

        mSpecialisedObject.userData = this;
    }

    bool Area2D::isOverlapping(const Area2D* other) const noexcept {
        return mSpecialisedObject.isOverlapping(&other->mSpecialisedObject);
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
