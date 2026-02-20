#include <m3ds/nodes/ui/Button.hpp>

namespace M3DS {
    const Style& Button::getStyle(const State state) const noexcept {
        return mBoxStyles[std::to_underlying(state)];
    }

    void Button::setStyle(const State state, const Style& to) noexcept {
        mBoxStyles[std::to_underlying(state)] = to;
        if (mState == state)
            queueResize();
    }

    Button::State Button::getState() const noexcept {
        return mState;
    }

    void Button::setState(const State to) noexcept {
        if (mState != to) {
            if (M3DS::getMinSize(getStyle(mState)) != M3DS::getMinSize(getStyle(to)))
                queueResize();
            mState = to;
        }
    }

    Label * Button::getLabel() noexcept {
        return mLabel;
    }

    const Label * Button::getLabelConst() const noexcept {
        return mLabel;
    }

    Failure Button::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(action) ||
            !serialiser.write(mState) ||
            !serialiser.write(mTouchActivated)
        )
            return Failure{ ErrorCode::file_write_fail };

        for (const Style& style : mBoxStyles) {
            if (const Failure failure = M3DS::serialise(style, serialiser))
                return failure;
        }

        return buttonPressed.serialise(this, serialiser);
    }

    Failure Button::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(action) ||
            !deserialiser.read(mState) ||
            !deserialiser.read(mTouchActivated)
        )
            return Failure{ ErrorCode::file_write_fail };

        for (Style& style : mBoxStyles) {
            if (const Failure failure = M3DS::deserialise(style, deserialiser))
                return failure;
        }

        return buttonPressed.deserialise(this, deserialiser);
    }

    void Button::input(Input::InputFrame& inputFrame) {
        MarginContainer::input(inputFrame);

        if (mState == State::locked)
            return;

        const auto& cursor = inputFrame.getCursor();
        const Vector2 screenPosition = getScreenPosition();

        const bool touchInside = cursor && (
            cursor->x >= screenPosition.x && cursor->x < screenPosition.x + getSize().x &&
            cursor->y >= screenPosition.y && cursor->y < screenPosition.y + getSize().y
        ) && !isOccluded(cursor.value());
        if (touchInside)
            inputFrame.consumeCursor();


        if (touchInside && inputFrame.consumePress(Input::Key::touch)) {
            Debug::log<2>("Button Pressed");
            mTouchActivated = true;
            setState(State::pressed);
            if (action == Action::emit_on_press)
                buttonPressed.emit();
        } else if (inputFrame.isUp(Input::Key::touch) && mState == State::pressed && mTouchActivated) {
            Debug::log<2>("Button Released");
            mTouchActivated = false;
            setState(State::normal);
            if (action == Action::emit_on_release)
                buttonPressed.emit();
        } else if (!touchInside && (mState == State::pressed) && inputFrame.isHeld(Input::Key::touch)) {
            Debug::log<2>("Unsetting Button");
            mTouchActivated = false;
            setState(State::normal);
        } else if (touchInside && mState == State::normal) {
            Debug::log<2>("Hovering Button");
            setState(State::hover);
        } else if (!touchInside) {
            setState(State::normal);
        }
    }

    void Button::draw(RenderTarget2D& target) {
        target.draw(getStyle(mState), getGlobalTransform(), getSize());

        MarginContainer::draw(target);
    }

    void Button::updateMinSize() noexcept {
        MarginContainer::updateMinSize();

        mInternalMinSize = max(mInternalMinSize, M3DS::getMinSize(getStyle(mState)));
    }

    REGISTER_NO_METHODS(Button);
    REGISTER_NO_MEMBERS(Button);
}
