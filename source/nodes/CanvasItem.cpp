#include <m3ds/nodes/CanvasItem.hpp>

#include <m3ds/nodes/Viewport.hpp>
#include <m3ds/nodes/2d/Camera2D.hpp>

namespace M3DS {
    void CanvasItem::setTranslation(const Vector2& to) noexcept {
        if (mTransform.position != to) {
            mTransform.position = to;
            setGlobalTransformDirty();
        }
    }

    void CanvasItem::setGlobalTranslation(const Vector2& to) noexcept {
        setTranslation(to - getGlobalTransform().position + mTransform.position);
    }

    void CanvasItem::setRotation(const Radians<float> to) noexcept {
        if (mTransform.rotation != to) {
            mTransform.rotation = to;
            setGlobalTransformDirty();
        }
    }

    void CanvasItem::setGlobalRotation(const Radians<float> to) noexcept {
        setRotation(to - getGlobalTransform().rotation + mTransform.rotation);
    }

    void CanvasItem::setScale(const Vector2& to) noexcept {
        if (mTransform.scale != to) {
            mTransform.scale = to;
            setGlobalTransformDirty();
        }
    }

    const Transform2D& CanvasItem::getTransform() const noexcept {
        return mTransform;
    }

    const Transform2D& CanvasItem::getGlobalTransform() const noexcept {
        if (mGlobalTransformDirty) {
            if (const auto* parentItem = object_cast<const CanvasItem*>(getParent())) {
                mGlobalTransform = parentItem->getGlobalTransform().offset(mTransform);
            } else {
                mGlobalTransform = mTransform;
            }
            mGlobalTransformDirty = false;
        }
        return mGlobalTransform;
    }

    const Vector2& CanvasItem::getTranslation() const noexcept {
        return mTransform.position;
    }

    Radians<float> CanvasItem::getRotation() const noexcept {
        return mTransform.rotation;
    }

    const Vector2& CanvasItem::getScale() const noexcept {
        return mTransform.scale;
    }

    Vector2 CanvasItem::getScreenPosition() const noexcept {
        if (const Viewport* viewport = getViewport()) {
            if (getCanvasLayer()) {
                return getGlobalTransform().position;
            }
            if (const Camera2D* camera = viewport->getCamera2D()) {
                return getGlobalTransform().offset(camera->getGlobalTransform().inverse()).position + static_cast<Vector2>(viewport->getSize() / 2);
            }
        }

        return getGlobalTransform().position;
    }

    Error CanvasItem::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node::serialise(file); error != Error::none)
            return error;

        if (!file.write(mTransform))
            return Error::file_write_fail;

        return Error::none;
    }

    Error CanvasItem::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Node::deserialise(file); error != Error::none)
            return error;

        if (!file.read(mTransform))
            return Error::file_read_fail;

        return Error::none;
    }

    void CanvasItem::afterTreeEnter() {
        Node::afterTreeEnter();

        mGlobalTransformDirty = true;
    }

    void CanvasItem::beforeTreeExit() {
        Node::beforeTreeExit();

        mGlobalTransformDirty = true;
    }

    void CanvasItem::setGlobalTransformDirty() noexcept {
        if (!mGlobalTransformDirty) {
            mGlobalTransformDirty = true;
            for (auto& child : getChildren()) {
                if (auto* c = object_cast<CanvasItem*>(child.get())) {
                    c->setGlobalTransformDirty();
                }
            }
        }
    }

    REGISTER_METHODS(
        CanvasItem,

        MUTABLE_METHOD(setTranslation),
        MUTABLE_METHOD(setGlobalTranslation),
        MUTABLE_METHOD(setRotation),
        MUTABLE_METHOD(setGlobalRotation),
        MUTABLE_METHOD(setScale),

        CONST_METHOD(getTransform),
        CONST_METHOD(getGlobalTransform),
        CONST_METHOD(getTranslation),
        CONST_METHOD(getRotation),
        CONST_METHOD(getScale),
        CONST_METHOD(getScreenPosition)
    );

    REGISTER_MEMBERS(
        CanvasItem,

        PRIVATE_MEMBER(translation, getTranslation, setTranslation),
        PRIVATE_MEMBER(rotation, getRotation, setRotation),
        PRIVATE_MEMBER(scale, getScale, setScale),
        PRIVATE_GET_ONLY_MEMBER(transform, getTransform)
    );
}
