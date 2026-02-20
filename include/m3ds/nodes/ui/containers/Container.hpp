#pragma once

#include <m3ds/nodes/ui/UINode.hpp>

namespace M3DS {
    class Container : public UINode {
        M_CLASS(Container, UINode)
    protected:
        Vector2 getChildrenMaxSize() noexcept;
    };
}
