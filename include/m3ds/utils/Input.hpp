#pragma once


#include <m3ds/spatial/Vector2.hpp>

namespace M3DS::Input {
    static constexpr Vector2 deadZone { 0.1f };

    enum class Key : std::uint32_t {
        a             = 1ul << 0,          ///< A
        b             = 1ul << 1,          ///< B
        select        = 1ul << 2,          ///< Select
        start         = 1ul << 3,          ///< Start
        d_right       = 1ul << 4,          ///< D-Pad Right
        d_left        = 1ul << 5,          ///< D-Pad Left
        d_up          = 1ul << 6,          ///< D-Pad Up
        d_down        = 1ul << 7,          ///< D-Pad Down
        r             = 1ul << 8,          ///< R
        l             = 1ul << 9,          ///< L
        x             = 1ul << 10,         ///< X
        y             = 1ul << 11,         ///< Y
        zl            = 1ul << 14,         ///< ZL (New 3DS only)
        zr            = 1ul << 15,         ///< ZR (New 3DS only)
        touch         = 1ul << 20,         ///< Touch (Not actually provided by HID)
        c_stick_right = 1ul << 24,         ///< C-Stick Right (New 3DS only)
        c_stick_left  = 1ul << 25,         ///< C-Stick Left (New 3DS only)
        c_stick_up    = 1ul << 26,         ///< C-Stick Up (New 3DS only)
        c_stick_down  = 1ul << 27,         ///< C-Stick Down (New 3DS only)
        c_pad_right   = 1ul << 28,         ///< Circle Pad Right
        c_pad_left    = 1ul << 29,         ///< Circle Pad Left
        c_pad_up      = 1ul << 30,         ///< Circle Pad Up
        c_pad_down    = 1ul << 31,         ///< Circle Pad Down

        // Generic catch-all directions
        up    = d_up    | c_pad_down,    ///< D-Pad Up or Circle Pad Up
        down  = d_down  | c_pad_down,    ///< D-Pad Down or Circle Pad Down
        left  = d_left  | c_pad_left,    ///< D-Pad Left or Circle Pad Left
        right = d_right | c_pad_right,   ///< D-Pad Right or Circle Pad Right
    };

    struct ControllerState {
        std::uint32_t pressed {};
        std::uint32_t held {};
        std::uint32_t released {};

        std::optional<Vector2> cursor {};

        Vector2 leftJoy {};
        Vector2 rightJoy {};

        explicit operator bool() const noexcept;
    };

    void update() noexcept;

    const ControllerState& getControllerState(std::size_t controller = 0) noexcept;

    bool isKeyPressed(Key key, std::size_t controller = 0) noexcept;
    bool isKeyHeld(Key key, std::size_t controller = 0) noexcept;
    bool isKeyReleased(Key key, std::size_t controller = 0) noexcept;

    const Vector2& getLeftJoy(std::size_t controller = 0) noexcept;
    const Vector2& getRightJoy(std::size_t controller = 0) noexcept;

    float getAxis(
        Key negativeAction,
        Key positiveAction,
        std::size_t controller = 0
    ) noexcept;

    Vector2 getVector(
        Key negativeActionX = Key::d_left,
        Key positiveActionX = Key::d_right,
        Key negativeActionY = Key::d_up,
        Key positiveActionY = Key::d_down,
        std::size_t controller = 0
    ) noexcept;
}
