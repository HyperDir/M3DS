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

    Failure BoxStyle::serialise(Serialiser& serialiser) const noexcept {
        if (
            !serialiser.write(colour) ||
            !serialiser.write(contentMargin)
        )
            return { ErrorCode::file_write_fail };

        const bool customTexture = texture != stretchPanelTexture;
        if (!serialiser.write(customTexture))
            return { ErrorCode::file_write_fail };

        if (customTexture)
            return texture.serialise(serialiser);

        return Success;
    }

    Failure BoxStyle::deserialise(Deserialiser& deserialiser) noexcept {
        if (
            !deserialiser.read(colour) ||
            !deserialiser.read(contentMargin)
        )
            return { ErrorCode::file_write_fail };

        bool customTexture {};
        if (!deserialiser.read(customTexture))
            return { ErrorCode::file_read_fail };

        if (customTexture)
            return texture.deserialise(deserialiser);
        
        return Success;
    }
}

