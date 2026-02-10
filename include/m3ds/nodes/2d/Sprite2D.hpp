#pragma once

#include <m3ds/nodes/2d/Node2D.hpp>

namespace M3DS {
    class Sprite2D : public Node2D {
        M_CLASS(Sprite2D, Node2D)
    public:
        SpriteSheet spritesheet {};
        std::uint32_t frame {};
        bool centre = true;

        Sprite2D() noexcept = default;
        explicit Sprite2D(SpriteSheet sheet) noexcept;
    protected:
        void draw(RenderTarget2D& target) override;
    };
}
