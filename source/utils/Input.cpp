#include <m3ds/utils/Input.hpp>

#ifdef __3DS__
#include <3ds.h>
#endif

#include <m3ds/utils/Debug.hpp>

#ifdef M3DS_SFML
#include <SFML/Window/Joystick.hpp>
#endif

namespace M3DS::Input {
    static std::array<ControllerState, 8> controllers {};

    ControllerState::operator bool() const noexcept {
        return (
            pressed ||
            held ||
            released ||
            cursor.has_value() ||
            leftJoy ||
            rightJoy
        );
    }

    void update() noexcept {
#ifdef __3DS__
        hidScanInput();

        ControllerState& state = controllers[0];

        state.pressed = hidKeysDown();
        state.held = hidKeysHeld();
        state.released = hidKeysUp();

        if (state.held & KEY_TOUCH) {
            touchPosition touchPos;
            hidTouchRead(&touchPos);
            state.cursor = { static_cast<float>(touchPos.px), static_cast<float>(touchPos.py) };
        } else {
            state.cursor = {};
        }

        circlePosition circlePos;
        hidCircleRead(&circlePos);
        state.leftJoy = Vector2{ static_cast<float>(circlePos.dx), static_cast<float>(-circlePos.dy) } / 150.f;

        if (std::abs(state.leftJoy.x) < deadZone.x) state.leftJoy.x = 0;
        if (std::abs(state.leftJoy.y) < deadZone.y) state.leftJoy.y = 0;

        state.leftJoy = state.leftJoy.clampToRadius(1.f);

        hidCstickRead(&circlePos);
        state.rightJoy = Vector2{ static_cast<float>(circlePos.dx), static_cast<float>(-circlePos.dy) } / 150.f;

        if (std::abs(state.rightJoy.x) < deadZone.x) state.rightJoy.x = 0;
        if (std::abs(state.rightJoy.y) < deadZone.y) state.rightJoy.y = 0;

        state.rightJoy = state.rightJoy.clampToRadius(1.f);
#elifdef M3DS_SFML
        sf::Joystick::update();

        for (unsigned int i{}; i < sf::Joystick::Count; ++i) {
            if (sf::Joystick::isConnected(i)) {
                std::cout << "Joystick connected!" << std::endl;
                std::cout << "Button count: " << sf::Joystick::getButtonCount(i) << std::endl;
            }
        }
#endif
    }

    const ControllerState& getControllerState(const std::size_t controller) noexcept {
        if (controller >= controllers.size()) {
            Debug::err("Invalid controller index: {}! Returning controller 0 instead!", controller);
            return controllers[0];
        }
        return controllers[controller];
    }

    bool isKeyPressed(const Key key, const std::size_t controller) noexcept {
        return static_cast<bool>(getControllerState(controller).pressed & std::to_underlying(key));
    }

    bool isKeyHeld(const Key key, const std::size_t controller) noexcept {
        return static_cast<bool>(getControllerState(controller).held & std::to_underlying(key));
    }

    bool isKeyReleased(const Key key, const std::size_t controller) noexcept {
        return static_cast<bool>(getControllerState(controller).released & std::to_underlying(key));
    }

    const Vector2& getLeftJoy(const std::size_t controller) noexcept {
        return getControllerState(controller).leftJoy;
    }

    const Vector2& getRightJoy(const std::size_t controller) noexcept {
        return getControllerState(controller).rightJoy;
    }

    float getAxis(
        const Key negativeAction,
        const Key positiveAction,
        const std::size_t controller
    ) noexcept {
        const ControllerState& state = getControllerState(controller);
        return static_cast<float>(
            static_cast<bool>(state.held & std::to_underlying(positiveAction)) -
            static_cast<bool>(state.held & std::to_underlying(negativeAction))
        );
    }

    Vector2 getVector(
        const Key negativeActionX,
        const Key positiveActionX,
        const Key negativeActionY,
        const Key positiveActionY,
        const std::size_t controller
    ) noexcept {
        return {
            getAxis(negativeActionX, positiveActionX, controller),
            getAxis(negativeActionY, positiveActionY, controller)
        };
    }
}
