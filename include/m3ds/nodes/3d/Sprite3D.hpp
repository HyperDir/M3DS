#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>

namespace M3DS {
    class Sprite3D : public Node3D {
        M_CLASS(Sprite3D, Node3D)
    public:
        SpriteSheet spritesheet {};
        std::uint32_t frame {};
        float pixelSize = 0.01f;

        bool cullBack = false;
        bool billboard = false;

        Sprite3D() noexcept = default;
        explicit Sprite3D(SpriteSheet sheet) noexcept;
    protected:
        void draw(RenderTarget3D& target) override;
    };
}
