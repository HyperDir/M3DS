#include <m3ds/nodes/3d/CollisionObject3D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void CollisionObject3D::setLayer(const std::uint32_t layer) noexcept {
        getCollisionObject()->setLayer(layer);
    }

    void CollisionObject3D::setMask(const std::uint32_t mask) noexcept {
        getCollisionObject()->setMask(mask);
    }

    std::uint32_t CollisionObject3D::getLayer() const noexcept {
        return getCollisionObject()->getLayer();
    }

    std::uint32_t CollisionObject3D::getMask() const noexcept {
        return getCollisionObject()->getMask();
    }

    void CollisionObject3D::setShape(const SPhys::Shape3D& shape) noexcept {
        getCollisionObject()->setLocalShape(shape);
    }

    const SPhys::Shape3D& CollisionObject3D::getShape() const noexcept {
        return getCollisionObject()->getLocalShape();
    }

    void CollisionObject3D::enableCollision() noexcept {
        getCollisionObject()->enable();
    }

    void CollisionObject3D::disableCollision() noexcept {
        getCollisionObject()->disable();
    }

    bool CollisionObject3D::isCollisionDisabled() const noexcept {
        return getCollisionObject()->isDisabled();
    }

    SPhys::CollisionObject3D* CollisionObject3D::getCollisionObject() noexcept {
        return mCollisionObject;
    }

    const SPhys::CollisionObject3D* CollisionObject3D::getCollisionObject() const noexcept {
        return mCollisionObject;
    }

    void CollisionObject3D::readback() noexcept {
        const SPhys::CollisionObject3D* object = getCollisionObject();
        setGlobalTranslation(object->getTranslation());
        setGlobalRotation(object->getRotation());
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

        return serialiseCollisionShape(getCollisionObject()->getLocalShape(), file);
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

        getCollisionObject()->setLocalShape(shape);

        return Error::none;
    }

    void CollisionObject3D::update(const Seconds<float> delta) {
        Node3D::update(delta);

        const Matrix4x4& xform = getGlobalTransform();
        getCollisionObject()->setTranslation(xform.getTranslation());
        getCollisionObject()->setRotation(xform.getRotation());
    }

    void CollisionObject3D::internaliseState() noexcept {
        mLayer = mCollisionObject->getLayer();
        mMask = mCollisionObject->getMask();

        mShape = mCollisionObject->getLocalShape();
        mCollisionDisabled = mCollisionObject->isDisabled();
    }

    void CollisionObject3D::externaliseState() noexcept {
        mCollisionObject->setLayer(mLayer);
        mCollisionObject->setMask(mMask);

        mCollisionObject->setLocalShape(mShape);

        if (mCollisionDisabled) mCollisionObject->disable();
        else mCollisionObject->enable();
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
