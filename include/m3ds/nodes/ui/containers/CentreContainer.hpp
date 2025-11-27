#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    class CentreContainer : public Container {
        M_CLASS(CentreContainer, Container)
    protected:
        void resize() noexcept override;
        void updateMinSize() noexcept override;
    };
}
