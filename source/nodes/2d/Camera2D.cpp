#include <m3ds/nodes/2d/Camera2D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void Camera2D::enable() noexcept {
        if (Viewport* viewport = getViewport())
            viewport->setCamera2d(this);
    }

    void Camera2D::disable() noexcept {
        if (Viewport* viewport = getViewport(); viewport && viewport->getCamera2D() == this)
            viewport->setCamera2d(nullptr);
    }

    bool Camera2D::isActive() const noexcept {
        if (const Viewport* viewport = getViewport())
            return viewport->getCamera2D() == this;
        return false;
    }

    Failure Camera2D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (!serialiser.write(isActive()))
            return Failure{ ErrorCode::file_write_fail };
        return Success;
    }

    Failure Camera2D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (!deserialiser.read(mQueuedActive))
            return Failure{ ErrorCode::file_read_fail };
        return Success;
    }

    void Camera2D::afterTreeEnter() {
        Node2D::afterTreeEnter();

        if (mQueuedActive) {
            mQueuedActive = false;
            enable();
        }
    }

    void Camera2D::beforeTreeExit() {
        Node2D::beforeTreeExit();

        disable();
    }

    REGISTER_METHODS(
        Camera2D,
        CONST_METHOD(isActive),
        MUTABLE_METHOD(enable),
        MUTABLE_METHOD(disable)
    );

    REGISTER_NO_MEMBERS(Camera2D);
}
