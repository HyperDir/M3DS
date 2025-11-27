#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    class PanelContainer : public Container {
        M_CLASS(PanelContainer, Container)
        Style mStyle {};
    public:
        void setStyle(Style to) noexcept;
        [[nodiscard]] const Style& getStyle() const noexcept;
    protected:
        void draw(RenderTarget2D& target) override;
        void updateMinSize() noexcept override;
        void resize() noexcept override;
    };
}
