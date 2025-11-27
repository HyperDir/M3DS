#pragma once

#include <m3ds/nodes/ui/containers/BoxContainer.hpp>

namespace M3DS {
    class HBoxContainer : public BoxContainer {
        M_CLASS(HBoxContainer, BoxContainer)
    public:
        HBoxContainer() noexcept;
    };
}
