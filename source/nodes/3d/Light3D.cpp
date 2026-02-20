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

    Failure Light3D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(colour) ||
            !serialiser.write(isActive())
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure Light3D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (!deserialiser.read(colour) || !deserialiser.read(mQueuedActive))
            return Failure{ ErrorCode::file_read_fail };

        return Success;
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
