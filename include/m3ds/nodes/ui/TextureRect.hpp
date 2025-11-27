#pragma once

#include <m3ds/nodes/ui/UINode.hpp>

namespace M3DS {
    class TextureRect : public UINode {
        M_CLASS(TextureRect, UINode)
        Texture mTexture {};
    public:
        std::uint32_t frame {};
        bool centre {};

        explicit TextureRect() noexcept = default;
        explicit TextureRect(Texture texture) noexcept;

        void setTexture(Texture texture) noexcept;
        const Texture& getTexture() const noexcept;
    protected:
        void draw(RenderTarget2D& target) override;

        void updateMinSize() noexcept override;
    };
}

