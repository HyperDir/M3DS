#include <m3ds/nodes/2d/KinematicBody2D.hpp>

namespace M3DS {
    KinematicBody2D::KinematicBody2D()
        : PhysicsBody2D(&mSpecialisedObject)
    {
        mSpecialisedObject.userData = this;
    }

    void KinematicBody2D::setVelocity(const PixelsPerSecond<Vector2>& to) noexcept {
        mSpecialisedObject.setVelocity(to);
    }

    void KinematicBody2D::setUpDirection(const Vector2& to) noexcept {
        mSpecialisedObject.setUpDirection(to);
    }

    void KinematicBody2D::setSlideOnSlope(const bool to) noexcept {
        mSpecialisedObject.setSlideOnSlope(to);
    }

    const PixelsPerSecond<Vector2>& KinematicBody2D::getVelocity() const noexcept {
        return mSpecialisedObject.getVelocity();
    }

    const Vector2& KinematicBody2D::getUpDirection() const noexcept {
        return mSpecialisedObject.getUpDirection();
    }

    bool KinematicBody2D::getSlideOnSlope() const noexcept {
        return mSpecialisedObject.getSlideOnSlope();
    }

    bool KinematicBody2D::isOnGround() const noexcept {
        return mSpecialisedObject.isOnGround();
    }

    struct SerialisationData {
        Vector2 velocity;
        Vector2 upDirection;
        bool slideOnSlope;
    };

    Error KinematicBody2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = PhysicsBody2D::serialise(file); error != Error::none)
            return error;

        const SerialisationData data {
            getVelocity(),
            getUpDirection(),
            getSlideOnSlope(),
        };

        if (!file.write(data))
            return Error::file_write_fail;

        return Error::none;
    }

    Error KinematicBody2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = PhysicsBody2D::deserialise(file); error != Error::none)
            return error;

        SerialisationData data;
        if (!file.read(data))
            return Error::file_read_fail;

        setVelocity(data.velocity);
        setUpDirection(data.upDirection);
        setSlideOnSlope(data.slideOnSlope);

        return Error::none;
    }

    REGISTER_METHODS(
        KinematicBody2D,
        MUTABLE_METHOD(setVelocity),
        MUTABLE_METHOD(setUpDirection),
        MUTABLE_METHOD(setSlideOnSlope),
        CONST_METHOD(getVelocity),
        CONST_METHOD(getUpDirection),
        CONST_METHOD(getSlideOnSlope),
        CONST_METHOD(isOnGround)
    );

    REGISTER_MEMBERS(
        KinematicBody2D,
        bindMember("velocity", &KinematicBody2D::getVelocity, &KinematicBody2D::setVelocity),
        bindMember("upDirection", &KinematicBody2D::getUpDirection, &KinematicBody2D::setUpDirection),
        bindMember("slideOnSlope", &KinematicBody2D::getSlideOnSlope, &KinematicBody2D::setSlideOnSlope),
        bindMember("isOnGround", &KinematicBody2D::isOnGround)
    );
}
