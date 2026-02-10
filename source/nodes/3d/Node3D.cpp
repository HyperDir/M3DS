#include <m3ds/nodes/3d/Node3D.hpp>

namespace M3DS {
    void Node3D::setTransform(const Matrix4x4& to) noexcept {
        if (to != mTransform) {
            setGlobalTransformDirty();
            mTransform = to;
        }
    }

    void Node3D::setTranslation(const Vector3& to) noexcept {
        if (mTransform.getTranslation() != to) {
            mTransform.setTranslation(to);
            setGlobalTransformDirty();
        }
    }

    void Node3D::setRotation([[maybe_unused]] const Quaternion& to) noexcept {
        mTransform.setRotation(to);
        setGlobalTransformDirty();
    }

    void Node3D::setScale([[maybe_unused]] const Vector3& to) noexcept {
        mTransform.setScale(to);
        setGlobalTransformDirty();
    }

    const Matrix4x4& Node3D::getTransform() const noexcept {
        return mTransform;
    }

    const Matrix4x4& Node3D::getGlobalTransform() const noexcept {
        if (mGlobalTransformDirty) {
            if (const auto* parentItem = object_cast<const Node3D*>(getParent())) {
                mGlobalTransform = parentItem->getGlobalTransform() * mTransform;
            } else {
                mGlobalTransform = mTransform;
            }
            mGlobalTransformDirty = false;
        }
        return mGlobalTransform;
    }

    Vector3 Node3D::getGlobalTranslation() const noexcept {
        return getGlobalTransform().getTranslation();
    }

    Vector3 Node3D::getTranslation() const noexcept {
        return mTransform.getTranslation();
    }

    Quaternion Node3D::getRotation() const noexcept {
        return mTransform.getRotation();
    }

    Vector3 Node3D::getScale() const noexcept {
        return mTransform.getScale();
    }

    void Node3D::setGlobalTransform(const Matrix4x4& to) noexcept {
        if (const auto* parent = object_cast<Node3D*>(getParent())) {
            setTransform(parent->getGlobalTransform().inverse() * to);
        } else {
            setTransform(to);
        }
    }

    void Node3D::setGlobalTranslation(const Metres<Vector3>& to) noexcept {
        if (object_cast<Node3D*>(getParent())) {
            Matrix4x4 xform = getGlobalTransform();
            xform.setTranslation(to);
            setGlobalTransform(xform);
        } else {
            setTranslation(to);
        }
    }

    void Node3D::setGlobalRotation([[maybe_unused]] const Quaternion& to) {
        if (object_cast<Node3D*>(getParent())) {
            Matrix4x4 xform = getGlobalTransform();
            xform.setRotation(to);
            setGlobalTransform(xform);
        } else {
            setRotation(to);
        }
    }

    Failure Node3D::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (!file.write(mTransform))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure Node3D::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (!file.read(mTransform))
            return Failure{ ErrorCode::file_read_fail };

        mGlobalTransformDirty = true;

        return Success;
    }

    void Node3D::setGlobalTransformDirty() noexcept {
        if (!mGlobalTransformDirty) {
            mGlobalTransformDirty = true;
            for (auto& child : getChildren()) {
                if (auto* c = object_cast<Node3D*>(child.get())) {
                    c->setGlobalTransformDirty();
                }
            }
        }
    }

    REGISTER_METHODS(
        Node3D,

        MUTABLE_METHOD(setTransform),
        MUTABLE_METHOD(setTranslation),
        MUTABLE_METHOD(setRotation),
        MUTABLE_METHOD(setScale),

        CONST_METHOD(getTransform),
        CONST_METHOD(getGlobalTransform),
        CONST_METHOD(getGlobalTranslation),

        CONST_METHOD(getTranslation),
        CONST_METHOD(getRotation),
        CONST_METHOD(getScale),

        MUTABLE_METHOD(setGlobalTransform),
        MUTABLE_METHOD(setGlobalTranslation),
        MUTABLE_METHOD(setGlobalRotation)
    );

    REGISTER_MEMBERS(
        Node3D,

        PRIVATE_MEMBER(translation, getTranslation, setTranslation),
        PRIVATE_MEMBER(rotation, getRotation, setRotation),
        PRIVATE_MEMBER(scale, getScale, setScale)
    );
}
