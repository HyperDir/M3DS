#include <m3ds/nodes/2d/CollisionObject2D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void CollisionObject2D::setLayer(const std::uint32_t layer) noexcept {
        mCollisionObject->setLayer(layer);
    }

    void CollisionObject2D::setMask(const std::uint32_t mask) noexcept {
        mCollisionObject->setMask(mask);
    }

    std::uint32_t CollisionObject2D::getLayer() const noexcept {
        return mCollisionObject->getLayer();
    }

    std::uint32_t CollisionObject2D::getMask() const noexcept {
        return mCollisionObject->getMask();
    }

    void CollisionObject2D::setShape(SPhys::Shape2D shape) noexcept {
        mCollisionObject->setLocalShape(std::move(shape));
    }

    const SPhys::Shape2D& CollisionObject2D::getShape() const noexcept {
        return mCollisionObject->getLocalShape();
    }

    void CollisionObject2D::enableCollision() noexcept {
        mCollisionObject->enable();
    }

    void CollisionObject2D::disableCollision() noexcept {
        mCollisionObject->disable();
    }

    bool CollisionObject2D::isCollisionDisabled() const noexcept {
        return mCollisionObject->isDisabled();
    }

    SPhys::CollisionObject2D* CollisionObject2D::getCollisionObject() noexcept {
        return mCollisionObject;
    }

    const SPhys::CollisionObject2D* CollisionObject2D::getCollisionObject() const noexcept {
        return mCollisionObject;
    }

    Error CollisionObject2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node2D::serialise(file); error != Error::none)
            return error;

        if (!file.write(getLayer()) || !file.write(getMask()) || !file.write(isCollisionDisabled()))
            return Error::file_write_fail;

        return serialiseCollisionShape(mCollisionObject->getLocalShape(), file);
    }

    Error CollisionObject2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Node2D::deserialise(file); error != Error::none)
            return error;

        std::uint32_t layer, mask;
        bool collisionDisabled;
        if (!file.read(layer) || !file.read(mask) || !file.read(collisionDisabled))
            return Error::file_read_fail;

        if (collisionDisabled)
            disableCollision();
        else
            enableCollision();

        setLayer(layer);
        setMask(mask);

        SPhys::Shape2D shape {};

        if (const Error error = deserialiseCollisionShape(shape, file); error != Error::none)
            return error;

        mCollisionObject->setLocalShape(shape);

        return Error::none;
    }

    void CollisionObject2D::update(const Seconds<float> delta) {
        Node2D::update(delta);

        const Transform2D& xform = getGlobalTransform();
        mCollisionObject->setTranslation(xform.position);
        mCollisionObject->setRotation(xform.rotation);
    }

    CollisionObject2D::CollisionObject2D(
        SPhys::CollisionObject2D* collisionObject
    ) noexcept : mCollisionObject(collisionObject) {}

    void CollisionObject2D::afterTreeEnter() {
        Node2D::afterTreeEnter();

        if (auto* viewport = getViewport())
            viewport->mPhysicsServer2D.registerObject(*mCollisionObject);
    }

    void CollisionObject2D::beforeTreeExit() {
        Node2D::beforeTreeExit();

        if (auto* viewport = getViewport())
            viewport->mPhysicsServer2D.unregisterObject(*mCollisionObject);
    }

    void CollisionObject2D::readback() noexcept {
        setGlobalTranslation(mCollisionObject->getTranslation());
        setGlobalRotation(mCollisionObject->getRotation());
    }

    Error serialiseCollisionShape(const SPhys::Shape2D& shape, const BinaryOutFileAccessor file) noexcept {
        if (!file.write(static_cast<std::uint8_t>(shape.index())))
            return Error::file_write_fail;
        if (!shape.visit(
            [&](const auto& s) {
                return file.write(s);
            }
        ))
            return Error::file_write_fail;
        return Error::none;
    }

    Error deserialiseCollisionShape(SPhys::Shape2D& shape, BinaryInFileAccessor file) noexcept {
        std::uint8_t index;
        if (!file.read(index))
            return Error::file_read_fail;

        shape = variantFromIndex<SPhys::Shape2D>(index);
        if (!shape.visit(
            [&](auto& s) {
                return file.read(s);
            }
        ))
            return Error::file_read_fail;
        return Error::none;
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
        bindMember("layer", &CollisionObject2D::getLayer, &CollisionObject2D::setLayer),
        bindMember("mask", &CollisionObject2D::getMask, &CollisionObject2D::setMask)
    );
}
