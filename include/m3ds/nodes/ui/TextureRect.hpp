#pragma once

#include <m3ds/nodes/ui/UINode.hpp>

namespace M3DS {
    class TextureRect : public UINode {
        M_CLASS(TextureRect, UINode)
        SpriteSheet mTexture {};
    public:
        std::uint32_t frame {};
        bool centre {};

        explicit TextureRect() noexcept = default;
        explicit TextureRect(SpriteSheet texture) noexcept;

        void setTexture(SpriteSheet texture) noexcept;
        const SpriteSheet& getTexture() const noexcept;
    protected:
        void draw(RenderTarget2D& target) override;

        void updateMinSize() noexcept override;
    };
}
