#include <m3ds/nodes/3d/Camera3D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void Camera3D::enable() noexcept {
        if (Viewport* viewport = getViewport())
            viewport->setCamera3d(this);
    }

    void Camera3D::disable() noexcept {
        if (Viewport* viewport = getViewport(); viewport && viewport->getCamera3D() == this)
            viewport->setCamera3d(nullptr);
    }

    bool Camera3D::isActive() const noexcept {
        if (const Viewport* viewport = getViewport())
            return viewport->getCamera3D() == this;
        return false;
    }

    Error Camera3D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node3D::serialise(file); error != Error::none)
            return error;

        if (!file.write(isActive()))
            return Error::file_write_fail;
        return Error::none;
    }

    Error Camera3D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Node3D::deserialise(file); error != Error::none)
            return error;

        if (!file.read(mQueuedActive))
            return Error::file_read_fail;
        return Error::none;
    }

    void Camera3D::afterTreeEnter() {
        Node3D::afterTreeEnter();

        if (mQueuedActive) {
            mQueuedActive = false;
            enable();
        }
    }

    void Camera3D::beforeTreeExit() {
        Node3D::beforeTreeExit();

        disable();
    }

    REGISTER_METHODS(
        Camera3D,
        MUTABLE_METHOD(enable),
        MUTABLE_METHOD(disable),
        CONST_METHOD(isActive)
    );

    REGISTER_NO_MEMBERS(Camera3D);
}
