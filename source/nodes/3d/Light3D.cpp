#include <m3ds/nodes/3d/Light3D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    bool Light3D::enable() noexcept {
        if (Viewport* viewport = getViewport()) {
            return viewport->addLight(this);
        }
        return false;
    }

    void Light3D::disable() noexcept {
        if (Viewport* viewport = getViewport()) {
            viewport->removeLight(this);
        }
    }

    bool Light3D::isActive() const noexcept {
        if (const Viewport* viewport = getViewport())
            return viewport->isLightActive(this);
        return false;
    }

    Error Light3D::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node3D::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(colour) ||
            !file.write(isActive())
        )
            return Error::file_write_fail;

        return Error::none;
    }

    Error Light3D::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Node3D::deserialise(file); error != Error::none)
            return error;

        if (!file.read(colour) || !file.read(mQueuedActive))
            return Error::file_read_fail;

        return Error::none;
    }

    void Light3D::afterTreeEnter() {
        Node3D::afterTreeEnter();

        if (mQueuedActive) {
            mQueuedActive = false;
            enable();
        }
    }

    void Light3D::beforeTreeExit() {
        Node3D::beforeTreeExit();

        disable();
    }

    REGISTER_METHODS(
        Light3D,
        MUTABLE_METHOD(enable),
        MUTABLE_METHOD(disable)
    );

    REGISTER_MEMBERS(
        Light3D,
        MEMBER(colour)
    );
}
