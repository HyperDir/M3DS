#pragma once

#include <m3ds/nodes/ui/Label.hpp>
#include <m3ds/nodes/ui/containers/MarginContainer.hpp>
#include <m3ds/types/Signal.hpp>

namespace M3DS {
    class Button : public MarginContainer {
        M_CLASS(Button, MarginContainer)
    public:
        enum class State : std::uint8_t {
            normal,
            hover,
            pressed,
            locked
        };

        enum class Action : std::uint8_t {
            emit_on_press,
            emit_on_release
        };

        Action action = Action::emit_on_release;
        Signal buttonPressed {};

        const Style& getStyle(State state) const noexcept;
        void setStyle(State state, const Style& to) noexcept;

        [[nodiscard]] State getState() const noexcept;
        void setState(State to) noexcept;

        Label* getLabel() noexcept;
        const Label* getLabelConst() const noexcept;
    protected:
        void input(Input::InputFrame& inputFrame) override;
        void draw(RenderTarget2D& target) override;
        void updateMinSize() noexcept override;
    private:
        State mState = State::normal;
        bool mTouchActivated = false;
        Label* mLabel = emplaceChild<Label, true>();

        std::array<Style, 4> mBoxStyles {
            BoxStyle { Colour(0xCCCCCCFF) },
            BoxStyle { Colour(0xAAAAAAFF) },
            BoxStyle { Colour(0x646464FF) },
            BoxStyle { Colour(0x323232FF) }
        };
    };
}
