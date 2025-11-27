#pragma once

#include <m3ds/nodes/ui/containers/BoxContainer.hpp>

namespace M3DS {
    class VBoxContainer : public BoxContainer {
        M_CLASS(VBoxContainer, BoxContainer)
    public:
        VBoxContainer() noexcept;
    };
}
