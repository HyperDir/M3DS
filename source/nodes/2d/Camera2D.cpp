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

    Error Camera2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node2D::serialise(file); error != Error::none)
            return error;

        if (!file.write(isActive()))
            return Error::file_write_fail;
        return Error::none;
    }

    Error Camera2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Node2D::deserialise(file); error != Error::none)
            return error;

        if (!file.read(mQueuedActive))
            return Error::file_read_fail;
        return Error::none;
    }

    void Camera2D::afterTreeEnter() {
        Node2D::afterTreeEnter();

        if (mQueuedActive) {
            mQueuedActive = false;
            enable();
        }
    }

    /*
    void Camera2D::draw(RenderTarget2D& target) noexcept {
        Node2D::draw(target);

        constexpr auto fromColour = Colours::green;
        constexpr auto toColour = Colours::turquoise;
        constexpr auto centreColour = Colours::white;

        const auto halfSize = static_cast<Vector2>(target.getSize() / 4);
        const Vector2 centre = getGlobalTransform().position;

        const float left = centre.x - halfSize.x;
        const float right = centre.x + halfSize.x;
        const float top = centre.y - halfSize.y;
        const float bottom = centre.y + halfSize.y;

        C2D_DrawLine(
            left,
            top,
            static_cast<std::uint32_t>(fromColour),
            right,
            top,
            static_cast<uint32_t>(toColour),
            1,
            1
        );

        C2D_DrawLine(
            left,
            top,
            static_cast<std::uint32_t>(fromColour),
            left,
            bottom,
            static_cast<uint32_t>(toColour),
            1,
            1
        );

        C2D_DrawLine(
            right,
            top,
            static_cast<uint32_t>(toColour),
            right,
            bottom,
            static_cast<std::uint32_t>(fromColour),
            1,
            1
        );

        C2D_DrawLine(
            left,
            bottom,
            static_cast<uint32_t>(toColour),
            right,
            bottom,
            static_cast<std::uint32_t>(fromColour),
            1,
            1
        );

        // C2D_DrawLine(
        //     left,
        //     top,
        //     static_cast<std::uint32_t>(fromColour),
        //     right,
        //     bottom,
        //     static_cast<uint32_t>(fromColour),
        //     1,
        //     1
        // );
        //
        // C2D_DrawLine(
        //     left,
        //     bottom,
        //     static_cast<std::uint32_t>(toColour),
        //     right,
        //     top,
        //     static_cast<uint32_t>(toColour),
        //     1,
        //     1
        // );
        C2D_DrawLine(
            left,
            top,
            static_cast<std::uint32_t>(fromColour),
            centre.x,
            centre.y,
            static_cast<uint32_t>(centreColour),
            1,
            1
        );

        C2D_DrawLine(
            right,
            bottom,
            static_cast<std::uint32_t>(fromColour),
            centre.x,
            centre.y,
            static_cast<uint32_t>(centreColour),
            1,
            1
        );

        C2D_DrawLine(
            left,
            bottom,
            static_cast<std::uint32_t>(toColour),
            centre.x,
            centre.y,
            static_cast<uint32_t>(centreColour),
            1,
            1
        );

        C2D_DrawLine(
            right,
            top,
            static_cast<std::uint32_t>(toColour),
            centre.x,
            centre.y,
            static_cast<uint32_t>(centreColour),
            1,
            1
        );

    }
    */

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
