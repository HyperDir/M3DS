#include <m3ds/types/Style.hpp>

namespace M3DS {
    constexpr std::array stretchPanelTextureData = std::to_array<unsigned char>({
#embed <gfx/StretchRect.bmp.t3x>
    });

    const Texture BoxStyle::stretchPanelTexture = Texture::load(stretchPanelTextureData).value();

    Vector2 BoxStyle::getMinSize() const noexcept {
        return {
            std::max(contentMargin.left + contentMargin.right, 2 * texture.getSize().x),
            std::max(contentMargin.top + contentMargin.bottom, 2 * texture.getSize().y)
        };
    }

    Failure BoxStyle::serialise(BinaryOutFileAccessor file) const noexcept {
        if (
            !file.write(colour) ||
            !file.write(contentMargin)
        )
            return { ErrorCode::file_write_fail };

        const bool customTexture = texture != stretchPanelTexture;
        if (!file.write(customTexture))
            return { ErrorCode::file_write_fail };

        if (customTexture)
            return texture.serialise(file);

        return Success;
    }

    Failure BoxStyle::deserialise(BinaryInFileAccessor file) noexcept {
        if (
            !file.read(colour) ||
            !file.read(contentMargin)
        )
            return { ErrorCode::file_write_fail };

        bool customTexture {};
        if (!file.read(customTexture))
            return { ErrorCode::file_read_fail };

        if (customTexture)
            return texture.deserialise(file);
        
        return Success;
    }
}

