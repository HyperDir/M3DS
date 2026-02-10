#include <m3ds/reference/resource/Font.hpp>

namespace M3DS {
    Font::Font(
        Texture image,
        const std::uint8_t lineHeight,
        const std::uint8_t lineSpacing,
        const std::uint8_t spaceSize,
        const std::uint8_t letterSpacing,
        const std::array<Glyph, 256>& glyphs
    ) noexcept
        : mTexture(std::move(image))
        , mLineHeight(lineHeight)
        , mLineSpacing(lineSpacing)
        , mSpaceSize(spaceSize)
        , mLetterSpacing(letterSpacing)
        , mGlyphs(glyphs)
    {}

    std::uint8_t Font::getLineHeight() const noexcept { return mLineHeight; }
    std::uint8_t Font::getLineSpacing() const noexcept { return mLineSpacing; }
    std::uint8_t Font::getSpaceSize() const noexcept { return mSpaceSize; }
    std::uint8_t Font::getLetterSpacing() const noexcept { return mLetterSpacing; }

    const Font::Glyph& Font::getGlyph(const char character) const noexcept {
        return mGlyphs[character];
    }

    const Texture& Font::getTexture() const noexcept { return mTexture; }

    std::shared_ptr<Font> Font::getDefaultFont() noexcept {
        static constexpr std::array<unsigned char, 509> defaultFontTexture {
            0x01, 0x00, 0x1B, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x04, 0x00, 0x04, 0x00, 0x00, 0x11, 0x00, 0x40, 0x00, 0x5F, 0x00, 0x60,
            0x00, 0xFF, 0x60, 0x00, 0xB0, 0x0F, 0xF0, 0x07, 0xB0, 0x23, 0xB0, 0x2B,
            0xFF, 0x00, 0x30, 0x3F, 0x00, 0x30, 0x47, 0x00, 0x30, 0x23, 0x00, 0x30,
            0x67, 0x01, 0x30, 0x5F, 0x00, 0xF0, 0x9F, 0x00, 0x30, 0x87, 0x00, 0x30,
            0xDF, 0xFF, 0x01, 0xB0, 0xFF, 0x00, 0x30, 0xF7, 0x01, 0xB0, 0xFF, 0x00,
            0xF0, 0xEF, 0x00, 0xB1, 0x5F, 0xF1, 0x73, 0x00, 0x31, 0x8F, 0x00, 0xB0,
            0xFF, 0xFF, 0x00, 0x70, 0x3F, 0x01, 0xF1, 0xFF, 0x00, 0xB0, 0x5B, 0x02,
            0x30, 0x3F, 0x00, 0x70, 0x5F, 0x02, 0x30, 0xFF, 0x00, 0x70, 0xF7, 0x00,
            0xB1, 0xF3, 0xFF, 0x01, 0xF1, 0xFF, 0x01, 0xF0, 0x5B, 0x00, 0xF3, 0x3F,
            0x02, 0x71, 0xFF, 0xF3, 0xB3, 0x01, 0x32, 0xA7, 0x02, 0x31, 0xBF, 0x01,
            0xB1, 0x03, 0xFF, 0x01, 0x30, 0x6B, 0x00, 0x34, 0x3F, 0x00, 0x34, 0x1B,
            0x01, 0xF0, 0xFF, 0x01, 0x72, 0xA3, 0x00, 0xB3, 0x4F, 0x01, 0xB1, 0xBF,
            0x02, 0xB0, 0xEF, 0xFF, 0x02, 0x34, 0xFF, 0x01, 0x31, 0xFF, 0x01, 0xB4,
            0xDF, 0x01, 0xB1, 0xAF, 0x01, 0x31, 0x67, 0x01, 0xB5, 0xD7, 0x00, 0xB3,
            0x07, 0x01, 0xF0, 0x03, 0xFF, 0x01, 0xF5, 0xA7, 0x02, 0x73, 0xC3, 0x00,
            0xF0, 0xB3, 0x01, 0x32, 0x1B, 0x03, 0x32, 0x03, 0x01, 0x71, 0xA3, 0x02,
            0x31, 0x3F, 0x01, 0xF4, 0x4F, 0xFF, 0x01, 0xF6, 0xBF, 0x01, 0x38, 0x07,
            0x02, 0xB7, 0x7F, 0x01, 0x37, 0x93, 0x01, 0x35, 0x33, 0x01, 0xF8, 0x0B,
            0x01, 0x72, 0x0F, 0x01, 0x79, 0x0B, 0xFF, 0x01, 0x73, 0xE3, 0x01, 0xF1,
            0x03, 0x01, 0x32, 0x23, 0x01, 0xF4, 0xAF, 0x02, 0x31, 0x1F, 0x01, 0x35,
            0xEF, 0x01, 0x71, 0xCF, 0x02, 0xB4, 0xC3, 0xFF, 0x01, 0xF9, 0xA3, 0x01,
            0x7B, 0x2B, 0x02, 0x39, 0x43, 0x01, 0xB2, 0xBF, 0x02, 0x37, 0xDF, 0x02,
            0x31, 0xBF, 0x02, 0xF3, 0x2F, 0x01, 0xB8, 0x3F, 0xFF, 0x01, 0xB9, 0x9B,
            0x02, 0x38, 0x8B, 0x00, 0xF8, 0xDF, 0x02, 0xF2, 0xEF, 0x01, 0x3A, 0x1F,
            0x02, 0x76, 0xE3, 0x01, 0xF9, 0x63, 0x02, 0xF0, 0x2B, 0xFF, 0x07, 0x30,
            0x27, 0x02, 0x33, 0xFF, 0x10, 0x02, 0xB0, 0x17, 0x01, 0xF2, 0xBF, 0x01,
            0x3F, 0x67, 0x01, 0x76, 0xDF, 0x02, 0xBF, 0x1F, 0x02, 0x78, 0x73, 0xFF,
            0x00, 0x70, 0x3F, 0x01, 0x7F, 0xDF, 0x03, 0x70, 0x3F, 0x00, 0xFE, 0xCF,
            0x01, 0xFF, 0x1F, 0x00, 0xF6, 0xB3, 0x01, 0xB6, 0xE3, 0x01, 0xFD, 0x3B,
            0xFF, 0x02, 0x3E, 0x5F, 0x02, 0x3A, 0x3F, 0x02, 0x76, 0x83, 0x01, 0xB0,
            0x5F, 0x00, 0xFB, 0xDF, 0x01, 0xF8, 0x9F, 0x01, 0x70, 0xFF, 0x02, 0xB1,
            0x1F, 0xFF, 0x00, 0x7E, 0xFF, 0x02, 0xFD, 0x63, 0x00, 0xFE, 0x5F, 0x01,
            0x78, 0x77, 0x02, 0x31, 0xFF, 0x02, 0xF0, 0x1F, 0x01, 0x31, 0xFF, 0x06,
            0xB7, 0x9B, 0xFF, 0x00, 0xFF, 0xE7, 0x01, 0xF1, 0xE7, 0x01, 0x7B, 0xDF,
            0x03, 0xB2, 0x1B, 0x02, 0xB2, 0x4F, 0x00, 0xFE, 0x0B, 0x01, 0xBD, 0x27,
            0x02, 0x36, 0xD3, 0xFF, 0x06, 0xF9, 0x3F, 0x07, 0x79, 0x97, 0x01, 0x7F,
            0x2B, 0x02, 0x72, 0xDB, 0x01, 0x3E, 0x4F, 0x02, 0x7B, 0x1F, 0x01, 0xB8,
            0x6F, 0x02, 0xBF, 0x3B, 0xFF, 0x02, 0x3B, 0x47, 0x02, 0xFF, 0x6F, 0x03,
            0xB1, 0xCF, 0x03, 0x7C, 0x5B, 0x02, 0x35, 0xEF, 0x02, 0x3E, 0xFF, 0x02,
            0xF2, 0x0B, 0x02, 0xF1, 0xAB, 0xFE, 0x01, 0xB1, 0xFB, 0x01, 0xB8, 0x4F,
            0x02, 0x79, 0x83, 0x05, 0xB2, 0x3F, 0x00, 0xFF, 0x87, 0x10, 0x04, 0x7D,
            0x87, 0x12, 0x15, 0xF0, 0x00
        };

        auto exp = Texture::load(std::span{ defaultFontTexture });
        if (!exp.has_value())
            Debug::terminate("Failed to load font with error: {}!", exp.error());

        static std::shared_ptr defaultFont = std::make_shared<Font>(
            std::move(exp.value()),
            5,
            2,
            4,
            1,
            std::array<Glyph, 256> {{
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                { {1, 5}, UVs(0, 0.015625, 0.796875, 0.71875) },
                { {3, 2}, UVs(0.03125, 0.078125, 0.796875, 0.765625) },
                { {5, 5}, UVs(0.09375, 0.171875, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.1875, 0.234375, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.25, 0.296875, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.3125, 0.359375, 0.796875, 0.71875) },
                { {1, 5}, UVs(0.375, 0.390625, 0.796875, 0.71875) },
                { {2, 5}, UVs(0.40625, 0.4375, 0.796875, 0.71875) },
                { {2, 5}, UVs(0.453125, 0.484375, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.5, 0.546875, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.5625, 0.609375, 0.796875, 0.71875) },
                { {2, 5}, UVs(0.625, 0.65625, 0.796875, 0.71875) },
                { {2, 5}, UVs(0.671875, 0.703125, 0.796875, 0.71875) },
                { {1, 5}, UVs(0.71875, 0.734375, 0.796875, 0.71875) },
                { {3, 5}, UVs(0.75, 0.796875, 0.796875, 0.71875) },
                { {3, 5}, UVs(0, 0.046875, 0.703125, 0.625) },
                { {3, 5}, UVs(0.0625, 0.109375, 0.703125, 0.625) },
                { {3, 5}, UVs(0.125, 0.171875, 0.703125, 0.625) },
                { {3, 5}, UVs(0.1875, 0.234375, 0.703125, 0.625) },
                { {3, 5}, UVs(0.25, 0.296875, 0.703125, 0.625) },
                { {3, 5}, UVs(0.3125, 0.359375, 0.703125, 0.625) },
                { {3, 5}, UVs(0.375, 0.421875, 0.703125, 0.625) },
                { {3, 5}, UVs(0.4375, 0.484375, 0.703125, 0.625) },
                { {3, 5}, UVs(0.5, 0.546875, 0.703125, 0.625) },
                { {3, 5}, UVs(0.5625, 0.609375, 0.703125, 0.625) },
                { {1, 5}, UVs(0.625, 0.640625, 0.703125, 0.625) },
                { {2, 5}, UVs(0.65625, 0.6875, 0.703125, 0.625) },
                { {2, 5}, UVs(0.703125, 0.734375, 0.703125, 0.625) },
                { {2, 5}, UVs(0.75, 0.78125, 0.703125, 0.625) },
                { {2, 5}, UVs(0.796875, 0.828125, 0.703125, 0.625) },
                { {3, 5}, UVs(0.84375, 0.890625, 0.703125, 0.625) },
                {},
                { {3, 5}, UVs(0, 0.046875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.0625, 0.109375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.125, 0.171875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.1875, 0.234375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.25, 0.296875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.3125, 0.359375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.375, 0.421875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.4375, 0.484375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.5, 0.546875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.5625, 0.609375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.625, 0.671875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.6875, 0.734375, 0.984375, 0.90625) },
                { {5, 5}, UVs(0.75, 0.828125, 0.984375, 0.90625) },
                { {4, 5}, UVs(0.84375, 0.90625, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.921875, 0.96875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0, 0.046875, 0.890625, 0.8125) },
                { {4, 5}, UVs(0.0625, 0.125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.140625, 0.1875, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.203125, 0.25, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.265625, 0.3125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.328125, 0.375, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.390625, 0.4375, 0.890625, 0.8125) },
                { {5, 5}, UVs(0.453125, 0.53125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.546875, 0.59375, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.609375, 0.65625, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.671875, 0.71875, 0.890625, 0.8125) },
                { {2, 5}, UVs(0, 0.03125, 0.609375, 0.53125) },
                { {3, 5}, UVs(0.046875, 0.09375, 0.609375, 0.53125) },
                { {2, 5}, UVs(0.109375, 0.140625, 0.609375, 0.53125) },
                { {3, 5}, UVs(0.15625, 0.203125, 0.609375, 0.53125) },
                { {3, 5}, UVs(0.21875, 0.265625, 0.609375, 0.53125) },
                { {2, 5}, UVs(0.28125, 0.3125, 0.609375, 0.53125) },
                { {3, 5}, UVs(0, 0.046875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.0625, 0.109375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.125, 0.171875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.1875, 0.234375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.25, 0.296875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.3125, 0.359375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.375, 0.421875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.4375, 0.484375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.5, 0.546875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.5625, 0.609375, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.625, 0.671875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.6875, 0.734375, 0.984375, 0.90625) },
                { {5, 5}, UVs(0.75, 0.828125, 0.984375, 0.90625) },
                { {4, 5}, UVs(0.84375, 0.90625, 0.984375, 0.90625) },
                { {3, 5}, UVs(0.921875, 0.96875, 0.984375, 0.90625) },
                { {3, 5}, UVs(0, 0.046875, 0.890625, 0.8125) },
                { {4, 5}, UVs(0.0625, 0.125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.140625, 0.1875, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.203125, 0.25, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.265625, 0.3125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.328125, 0.375, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.390625, 0.4375, 0.890625, 0.8125) },
                { {5, 5}, UVs(0.453125, 0.53125, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.546875, 0.59375, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.609375, 0.65625, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.671875, 0.71875, 0.890625, 0.8125) },
                { {3, 5}, UVs(0.328125, 0.375, 0.609375, 0.53125) },
                { {1, 5}, UVs(0.390625, 0.40625, 0.609375, 0.53125) },
                { {3, 5}, UVs(0.421875, 0.46875, 0.609375, 0.53125) },
                { {4, 5}, UVs(0.484375, 0.546875, 0.609375, 0.53125) },
            }}
        );
        return defaultFont;
    }

    Failure Font::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = mTexture.serialise(file))
            return failure;

        if (
            !file.write(mLineHeight) ||
            !file.write(mLineSpacing) ||
            !file.write(mSpaceSize) ||
            !file.write(mLetterSpacing) ||
            !file.write(mGlyphs)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure Font::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = mTexture.deserialise(file))
            return failure;

        if (
            !file.read(mLineHeight) ||
            !file.read(mLineSpacing) ||
            !file.read(mSpaceSize) ||
            !file.read(mLetterSpacing) ||
            !file.read(mGlyphs)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }



    REGISTER_NO_METHODS(Font);
    REGISTER_NO_MEMBERS(Font);
}
