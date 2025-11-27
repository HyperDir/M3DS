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

    Error Button::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = MarginContainer::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(action) ||
            !file.write(mState) ||
            !file.write(mTouchActivated)
        )
            return Error::file_write_fail;

        for (const Style& style : mBoxStyles) {
            if (const Error error = M3DS::serialise(style, file); error != Error::none)
                return error;
        }

        return buttonPressed.serialise(file);
    }

    Error Button::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = MarginContainer::deserialise(file); error != Error::none)
            return error;

        if (
            !file.read(action) ||
            !file.read(mState) ||
            !file.read(mTouchActivated)
        )
            return Error::file_write_fail;

        for (Style& style : mBoxStyles) {
            if (const Error error = M3DS::deserialise(style, file); error != Error::none)
                return error;
        }

        return buttonPressed.deserialise(file);
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
                buttonPressed.emit(this);
        } else if (inputFrame.isUp(Input::Key::touch) && mState == State::pressed && mTouchActivated) {
            Debug::log<2>("Button Released");
            mTouchActivated = false;
            setState(State::normal);
            if (action == Action::emit_on_release)
                buttonPressed.emit(this);
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
