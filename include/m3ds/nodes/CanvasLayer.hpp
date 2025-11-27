#pragma once

#include <m3ds/nodes/Node.hpp>

namespace M3DS {
    class CanvasLayer : public Node {
        M_CLASS(CanvasLayer, Node)
    public:
        CanvasLayer();
    protected:
        void draw(RenderTarget2D& target) override;
    };
}
