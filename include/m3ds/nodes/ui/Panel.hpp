#pragma once

#include <m3ds/nodes/ui/UINode.hpp>

namespace M3DS {
    class Panel : public UINode {
        M_CLASS(Panel, UINode)
        Style mStyle {};
    public:
        explicit Panel(Style style = {}) noexcept;

        void setStyle(Style to) noexcept;
        [[nodiscard]] const Style& getStyle() const noexcept;
    protected:
        void draw(RenderTarget2D& target) override;

        void updateMinSize() noexcept override;
    };
}
