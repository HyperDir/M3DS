#include <m3ds/nodes/2d/KinematicBody2D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void KinematicBody2D::setVelocity(const Vector2& to) noexcept {
        mAccessor->setVelocity(to);
    }

    void KinematicBody2D::setUpDirection(const Vector2& to) noexcept {
        mAccessor->setUpDirection(to);
    }

    void KinematicBody2D::setSlideOnSlope(const bool to) noexcept {
        mAccessor->setSlideOnSlope(to);
    }

    const MetresPerSecond<Vector2>& KinematicBody2D::getVelocity() const noexcept {
        return mAccessor->getVelocity();
    }

    const Vector2& KinematicBody2D::getUpDirection() const noexcept {
        return mAccessor->getUpDirection();
    }

    bool KinematicBody2D::getSlideOnSlope() const noexcept {
        return mAccessor->getSlideOnSlope();
    }

    bool KinematicBody2D::isOnGround() const noexcept {
        return mAccessor->isOnGround();
    }

    void KinematicBody2D::afterTreeEnter() {
        PhysicsBody2D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer2d().emplaceKinematicBody();
        mAccessor->userData = this;

        updateCollisionObject(mAccessor.get());
        externaliseState();

        mAccessor->setSlideOnSlope(mSerialisationData.slideOnSlope);
        mAccessor->setUpDirection(mSerialisationData.upDirection);
    }

    void KinematicBody2D::beforeTreeExit() {
        PhysicsBody2D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer2d().eraseKinematicBody(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    void KinematicBody2D::internaliseState() noexcept {
        PhysicsBody2D::internaliseState();

        mSerialisationData.velocity = getVelocity();
        mSerialisationData.upDirection = getUpDirection();
        mSerialisationData.slideOnSlope = getSlideOnSlope();
    }

    void KinematicBody2D::externaliseState() noexcept {
        PhysicsBody2D::externaliseState();

        setVelocity(mSerialisationData.velocity);
        setUpDirection(mSerialisationData.upDirection);
        setSlideOnSlope(mSerialisationData.slideOnSlope);
    }

    Failure KinematicBody2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        const InternalData data {
            getVelocity(),
            getUpDirection(),
            getSlideOnSlope(),
        };

        if (!file.write(data))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure KinematicBody2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        InternalData data;
        if (!file.read(data))
            return Failure{ ErrorCode::file_read_fail };

        setVelocity(data.velocity);
        setUpDirection(data.upDirection);
        setSlideOnSlope(data.slideOnSlope);

        return Success;
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
