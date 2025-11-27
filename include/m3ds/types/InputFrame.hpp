#pragma once

#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/utils/Input.hpp>

namespace M3DS::Input {
    class InputFrame {
    public:
        explicit constexpr InputFrame(std::uint32_t controller = 0) noexcept;
        explicit constexpr InputFrame(const ControllerState& state) noexcept;

        [[nodiscard]] constexpr bool isDown(Key key) const noexcept;
        [[nodiscard]] constexpr bool isHeld(Key key) const noexcept;
        [[nodiscard]] constexpr bool isUp(Key key) const noexcept;

        constexpr bool consumePress(Key key) noexcept;

        [[nodiscard]] constexpr const Vector2& getLeftJoy() const noexcept;
        [[nodiscard]] constexpr const Vector2& getRightJoy() const noexcept;

        [[nodiscard]] constexpr std::optional<Vector2> getCursor() const noexcept;
        constexpr std::optional<Vector2> consumeCursor() noexcept;

        explicit constexpr operator bool() const noexcept;
    private:
        ControllerState mState {};
        std::uint32_t player {};

        std::uint32_t consumedInputs {};
        bool cursorConsumed {};
    };

    constexpr InputFrame::InputFrame(const std::uint32_t controller) noexcept
        : mState(getControllerState(controller))
    {}

    constexpr InputFrame::InputFrame(const ControllerState& state) noexcept
        : mState(state)
    {}

    constexpr bool InputFrame::isDown(const Key key) const noexcept {
        return (mState.pressed & std::to_underlying(key)) && !(consumedInputs & std::to_underlying(key));
    }

    constexpr bool InputFrame::isHeld(const Key key) const noexcept {
        return mState.held & std::to_underlying(key);
    }

    constexpr bool InputFrame::isUp(const Key key) const noexcept {
        return mState.released & std::to_underlying(key);
    }

    constexpr bool InputFrame::consumePress(const Key key) noexcept {
        const bool isDown = mState.pressed & std::to_underlying(key);
        consumedInputs |= mState.pressed & std::to_underlying(key);
        return isDown;
    }

    constexpr const Vector2& InputFrame::getLeftJoy() const noexcept {
        return mState.leftJoy;
    }

    constexpr const Vector2& InputFrame::getRightJoy() const noexcept {
        return mState.rightJoy;
    }

    constexpr std::optional<Vector2> InputFrame::getCursor() const noexcept {
        if (cursorConsumed)
            return {};

        return mState.cursor;
    }

    constexpr std::optional<Vector2> InputFrame::consumeCursor() noexcept {
        if (cursorConsumed)
            return {};

        cursorConsumed = true;
        return mState.cursor;
    }

    constexpr InputFrame::operator bool() const noexcept {
        return static_cast<bool>(mState);
    }
}
