#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    class FillContainer : public Container {
        M_CLASS(FillContainer, Container)
    protected:
        void updateMinSize() noexcept override;
        void resize() noexcept override;
    };
}
