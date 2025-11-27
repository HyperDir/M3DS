#include <m3ds/nodes/3d/KinematicBody3D.hpp>

namespace M3DS {
    KinematicBody3D::KinematicBody3D()
        : PhysicsBody3D(&mSpecialisedObject)
    {
        mSpecialisedObject.userData = this;
    }

    void KinematicBody3D::setVelocity(const Vector3& to) noexcept {
        mSpecialisedObject.setVelocity(to);
    }

    void KinematicBody3D::setUpDirection(const Vector3& to) noexcept {
        mSpecialisedObject.setUpDirection(to);
    }

    void KinematicBody3D::setSlideOnSlope(const bool to) noexcept {
        mSpecialisedObject.setSlideOnSlope(to);
    }

    const MetresPerSecond<Vector3>& KinematicBody3D::getVelocity() const noexcept {
        return mSpecialisedObject.getVelocity();
    }

    const Vector3& KinematicBody3D::getUpDirection() const noexcept {
        return mSpecialisedObject.getUpDirection();
    }

    bool KinematicBody3D::getSlideOnSlope() const noexcept {
        return mSpecialisedObject.getSlideOnSlope();
    }

    bool KinematicBody3D::isOnGround() const noexcept {
        return mSpecialisedObject.isOnGround();
    }

    struct SerialisationData {
        Vector3 velocity;
        Vector3 upDirection;
        bool slideOnSlope;
    };

    Error KinematicBody3D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = PhysicsBody3D::serialise(file); error != Error::none)
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

    Error KinematicBody3D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = PhysicsBody3D::deserialise(file); error != Error::none)
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
        KinematicBody3D,
        MUTABLE_METHOD(setVelocity),
        MUTABLE_METHOD(setUpDirection),
        MUTABLE_METHOD(setSlideOnSlope),
        CONST_METHOD(getVelocity),
        CONST_METHOD(getUpDirection),
        CONST_METHOD(getSlideOnSlope),
        CONST_METHOD(isOnGround)
    );

    REGISTER_MEMBERS(
        KinematicBody3D,
        bindMember("velocity", &KinematicBody3D::getVelocity, &KinematicBody3D::setVelocity),
        bindMember("upDirection", &KinematicBody3D::getUpDirection, &KinematicBody3D::setUpDirection),
        bindMember("slideOnSlope", &KinematicBody3D::getSlideOnSlope, &KinematicBody3D::setSlideOnSlope),
        bindMember("isOnGround", &KinematicBody3D::isOnGround)
    );
}
