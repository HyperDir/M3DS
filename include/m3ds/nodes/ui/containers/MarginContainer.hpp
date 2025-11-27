#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>
#include <m3ds/types/Margin.hpp>

namespace M3DS {
    class MarginContainer : public Container {
        M_CLASS(MarginContainer, Container)
    public:
        [[nodiscard]] const Margin& getMargin() const noexcept;
        void setMargin(const Margin& to) noexcept;
    protected:
        void updateMinSize() noexcept override;
        void resize() noexcept override;
    private:
        Margin mMargin { 4, 4, 4, 4 };
    };
}
