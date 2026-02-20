#include <m3ds/nodes/3d/KinematicBody3D.hpp>

#include "m3ds/nodes/Viewport.hpp"

namespace M3DS {
    void KinematicBody3D::setVelocity(const Vector3& to) noexcept {
        mAccessor->setVelocity(to);
    }

    void KinematicBody3D::setUpDirection(const Vector3& to) noexcept {
        mAccessor->setUpDirection(to);
    }

    void KinematicBody3D::setSlideOnSlope(const bool to) noexcept {
        mAccessor->setSlideOnSlope(to);
    }

    const MetresPerSecond<Vector3>& KinematicBody3D::getVelocity() const noexcept {
        return mAccessor->getVelocity();
    }

    const Vector3& KinematicBody3D::getUpDirection() const noexcept {
        return mAccessor->getUpDirection();
    }

    bool KinematicBody3D::getSlideOnSlope() const noexcept {
        return mAccessor->getSlideOnSlope();
    }

    bool KinematicBody3D::isOnGround() const noexcept {
        return mAccessor->isOnGround();
    }

    void KinematicBody3D::afterTreeEnter() {
        PhysicsBody3D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer3d().emplaceKinematicBody();
        mAccessor->userData = this;

        updateCollisionObject(mAccessor.get());
        externaliseState();

        mAccessor->setSlideOnSlope(mSerialisationData.slideOnSlope);
        mAccessor->setUpDirection(mSerialisationData.upDirection);
    }

    void KinematicBody3D::beforeTreeExit() {
        PhysicsBody3D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer3d().eraseKinematicBody(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    void KinematicBody3D::internaliseState() noexcept {
        PhysicsBody3D::internaliseState();

        mSerialisationData.velocity = getVelocity();
        mSerialisationData.upDirection = getUpDirection();
        mSerialisationData.slideOnSlope = getSlideOnSlope();
    }

    void KinematicBody3D::externaliseState() noexcept {
        PhysicsBody3D::externaliseState();

        setVelocity(mSerialisationData.velocity);
        setUpDirection(mSerialisationData.upDirection);
        setSlideOnSlope(mSerialisationData.slideOnSlope);
    }

    Failure KinematicBody3D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        const InternalData data {
            getVelocity(),
            getUpDirection(),
            getSlideOnSlope(),
        };

        if (!serialiser.write(data))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure KinematicBody3D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        InternalData data;
        if (!deserialiser.read(data))
            return Failure{ ErrorCode::file_read_fail };

        setVelocity(data.velocity);
        setUpDirection(data.upDirection);
        setSlideOnSlope(data.slideOnSlope);

        return Success;
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
