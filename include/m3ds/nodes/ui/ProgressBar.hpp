#pragma once

#include <m3ds/nodes/ui/UINode.hpp>

namespace M3DS {
    enum class FillMode : std::uint8_t {
        left_to_right,
        right_to_left
    };

    class ProgressBar : public UINode {
        M_CLASS(ProgressBar, UINode)
    public:
        FillMode fillMode = FillMode::left_to_right;
        float value = 0;
        float minValue = 0;
        float maxValue = 100;
        Colour backgroundColour = Colours::dark_grey;
        Colour fillColour = Colours::red;
    protected:
        void updateMinSize() noexcept override;

        void draw(RenderTarget2D& target) override;
    };
}

