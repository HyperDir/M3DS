#include <m3ds/nodes/2d/CollisionObject2D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void CollisionObject2D::setLayer(const std::uint32_t layer) noexcept {
        getCollisionObject()->setLayer(layer);
    }

    void CollisionObject2D::setMask(const std::uint32_t mask) noexcept {
        getCollisionObject()->setMask(mask);
    }

    std::uint32_t CollisionObject2D::getLayer() const noexcept {
        return getCollisionObject()->getLayer();
    }

    std::uint32_t CollisionObject2D::getMask() const noexcept {
        return getCollisionObject()->getMask();
    }

    void CollisionObject2D::setShape(const SPhys::Shape2D& shape) noexcept {
        getCollisionObject()->setLocalShape(shape);
    }

    const SPhys::Shape2D& CollisionObject2D::getShape() const noexcept {
        return getCollisionObject()->getLocalShape();
    }

    void CollisionObject2D::enableCollision() noexcept {
        getCollisionObject()->enable();
    }

    void CollisionObject2D::disableCollision() noexcept {
        getCollisionObject()->disable();
    }

    bool CollisionObject2D::isCollisionDisabled() const noexcept {
        return getCollisionObject()->isDisabled();
    }

    SPhys::CollisionObject2D* CollisionObject2D::getCollisionObject() noexcept {
        return mCollisionObject;
    }

    const SPhys::CollisionObject2D* CollisionObject2D::getCollisionObject() const noexcept {
        return mCollisionObject;
    }

    void CollisionObject2D::readback() noexcept {
        const SPhys::CollisionObject2D* object = getCollisionObject();
        setGlobalTranslation(object->getTranslation());
        setGlobalRotation(object->getRotation());
    }

    Failure serialiseCollisionShape(const SPhys::Shape2D& shape, const BinaryOutFileAccessor file) noexcept {
        if (!file.write(static_cast<std::uint8_t>(shape.index())))
            return Failure{ ErrorCode::file_write_fail };
        if (!shape.visit(
            [&](const auto& s) {
                return file.write(s);
            }
        ))
            return Failure{ ErrorCode::file_write_fail };
        return Success;
    }

    Failure deserialiseCollisionShape(SPhys::Shape2D& shape, BinaryInFileAccessor file) noexcept {
        std::uint8_t index;
        if (!file.read(index))
            return Failure{ ErrorCode::file_read_fail };

        shape = variantFromIndex<SPhys::Shape2D>(index);
        if (!shape.visit(
            [&](auto& s) {
                return file.read(s);
            }
        ))
            return Failure{ ErrorCode::file_read_fail };
        return Success;
    }

    Failure CollisionObject2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (!file.write(getLayer()) || !file.write(getMask()) || !file.write(isCollisionDisabled()))
            return Failure{ ErrorCode::file_write_fail };

        return serialiseCollisionShape(getCollisionObject()->getLocalShape(), file);
    }

    Failure CollisionObject2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        std::uint32_t layer, mask;
        bool collisionDisabled;
        if (!file.read(layer) || !file.read(mask) || !file.read(collisionDisabled))
            return Failure{ ErrorCode::file_read_fail };

        if (collisionDisabled)
            disableCollision();
        else
            enableCollision();

        setLayer(layer);
        setMask(mask);

        SPhys::Shape2D shape {};

        if (const Failure failure = deserialiseCollisionShape(shape, file))
            return failure;

        getCollisionObject()->setLocalShape(shape);

        return Success;
    }

    void CollisionObject2D::update(const Seconds<float> delta) {
        Node2D::update(delta);

        const Transform2D& xform = getGlobalTransform();
        mCollisionObject->setTranslation(xform.position);
        mCollisionObject->setRotation(xform.rotation);
    }

    void CollisionObject2D::internaliseState() noexcept {
        mLayer = mCollisionObject->getLayer();
        mMask = mCollisionObject->getMask();

        mShape = mCollisionObject->getLocalShape();
        mCollisionDisabled = mCollisionObject->isDisabled();
    }

    void CollisionObject2D::externaliseState() noexcept {
        mCollisionObject->setLayer(mLayer);
        mCollisionObject->setMask(mMask);

        mCollisionObject->setLocalShape(mShape);

        if (mCollisionDisabled) mCollisionObject->disable();
        else mCollisionObject->enable();
    }

    REGISTER_METHODS(
        CollisionObject2D,

        MUTABLE_METHOD(setLayer),
        MUTABLE_METHOD(setMask),
        MUTABLE_METHOD(enableCollision),
        MUTABLE_METHOD(disableCollision),
        CONST_METHOD(getLayer),
        CONST_METHOD(getMask),
        CONST_METHOD(isCollisionDisabled)
    );

    REGISTER_MEMBERS(
        CollisionObject2D,

        PRIVATE_MEMBER(layer, getLayer, setLayer),
        PRIVATE_MEMBER(mask, getMask, setMask)
    );
}
