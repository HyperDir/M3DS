#pragma once

#include <m3ds/reference/Resource.hpp>
#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/utils/Texture.hpp>

namespace M3DS {
    class Font : public Resource {
        M_CLASS(Font, Resource)
    public:
        struct Glyph {
            Vec2<std::uint16_t> size {};
            UVs uvs {};
        };

        [[nodiscard]] Font() noexcept = default;
        [[nodiscard]] Font(
            Texture image,
            std::uint8_t lineHeight,
            std::uint8_t lineSpacing,
            std::uint8_t spaceSize,
            std::uint8_t letterSpacing,
            const std::array<Glyph, 256>& glyphs
        ) noexcept;

        [[nodiscard]] static std::shared_ptr<Font> getDefaultFont() noexcept;

        [[nodiscard]] std::uint8_t getLineHeight() const noexcept;
        [[nodiscard]] std::uint8_t getLineSpacing() const noexcept;
        [[nodiscard]] std::uint8_t getSpaceSize() const noexcept;
        [[nodiscard]] std::uint8_t getLetterSpacing() const noexcept;
        [[nodiscard]] const Glyph& getGlyph(char character) const noexcept;
        [[nodiscard]] const Texture& getTexture() const noexcept;
    private:
        Texture mTexture {};

        std::uint8_t mLineHeight = 5;
        std::uint8_t mLineSpacing = 2;
        std::uint8_t mSpaceSize = 4;
        std::uint8_t mLetterSpacing = 1;

        std::array<Glyph, 256> mGlyphs {};
    };
}
