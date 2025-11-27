#include <m3ds/nodes/3d/CollisionObject3D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void CollisionObject3D::setLayer(const std::uint32_t layer) noexcept {
        mCollisionObject->setLayer(layer);
    }

    void CollisionObject3D::setMask(const std::uint32_t mask) noexcept {
        mCollisionObject->setMask(mask);
    }

    std::uint32_t CollisionObject3D::getLayer() const noexcept {
        return mCollisionObject->getLayer();
    }

    std::uint32_t CollisionObject3D::getMask() const noexcept {
        return mCollisionObject->getMask();
    }

    void CollisionObject3D::setShape(SPhys::Shape3D shape) noexcept {
        mCollisionObject->setLocalShape(std::move(shape));
    }

    const SPhys::Shape3D& CollisionObject3D::getShape() const noexcept {
        return mCollisionObject->getLocalShape();
    }

    void CollisionObject3D::enableCollision() noexcept {
        mCollisionObject->enable();
    }

    void CollisionObject3D::disableCollision() noexcept {
        mCollisionObject->disable();
    }

    bool CollisionObject3D::isCollisionDisabled() const noexcept {
        return mCollisionObject->isDisabled();
    }

    SPhys::CollisionObject3D* CollisionObject3D::getCollisionObject() noexcept {
        return mCollisionObject;
    }

    const SPhys::CollisionObject3D* CollisionObject3D::getCollisionObject() const noexcept {
        return mCollisionObject;
    }

    Error serialiseCollisionShape(const SPhys::Shape3D& shape, const BinaryOutFileAccessor file) noexcept {
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

    Error deserialiseCollisionShape(SPhys::Shape3D& shape, BinaryInFileAccessor file) noexcept {
        std::uint8_t index;
        if (!file.read(index))
            return Error::file_read_fail;

        shape = variantFromIndex<SPhys::Shape3D>(index);
        if (!shape.visit(
            [&](auto& s) {
                return file.read(s);
            }
        ))
            return Error::file_read_fail;
        return Error::none;
    }

    Error CollisionObject3D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node3D::serialise(file); error != Error::none)
            return error;

        if (!file.write(getLayer()) || !file.write(getMask()) || !file.write(isCollisionDisabled()))
            return Error::file_write_fail;

        return serialiseCollisionShape(mCollisionObject->getLocalShape(), file);
    }

    Error CollisionObject3D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Node3D::deserialise(file); error != Error::none)
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

        SPhys::Shape3D shape {};

        if (const Error error = deserialiseCollisionShape(shape, file); error != Error::none)
            return error;

        mCollisionObject->setLocalShape(shape);

        return Error::none;
    }

    void CollisionObject3D::update(const Seconds<float> delta) {
        Node3D::update(delta);

        const Matrix4x4& xform = getGlobalTransform();
        mCollisionObject->setTranslation(xform.getTranslation());
        mCollisionObject->setRotation(xform.getRotation());
    }

    CollisionObject3D::CollisionObject3D(
        SPhys::CollisionObject3D* collisionObject
    ) noexcept : mCollisionObject(collisionObject) {}

    void CollisionObject3D::afterTreeEnter() {
        Node3D::afterTreeEnter();

        if (auto* viewport = getViewport())
            viewport->mPhysicsServer3D.registerObject(*mCollisionObject);
    }

    void CollisionObject3D::beforeTreeExit() {
        Node3D::beforeTreeExit();

        if (auto* viewport = getViewport())
            viewport->mPhysicsServer3D.unregisterObject(*mCollisionObject);
    }

    void CollisionObject3D::readback() noexcept {
        setGlobalTranslation(mCollisionObject->getTranslation());
        setGlobalRotation(mCollisionObject->getRotation());
    }

    REGISTER_METHODS(
        CollisionObject3D,
        MUTABLE_METHOD(setLayer),
        MUTABLE_METHOD(setMask),
        MUTABLE_METHOD(enableCollision),
        MUTABLE_METHOD(disableCollision),
        CONST_METHOD(getLayer),
        CONST_METHOD(getMask),
        CONST_METHOD(isCollisionDisabled)
    );

    REGISTER_MEMBERS(
        CollisionObject3D,
        PRIVATE_MEMBER(layer, getLayer, setLayer),
        PRIVATE_MEMBER(mask, getMask, setMask)
    );
}
