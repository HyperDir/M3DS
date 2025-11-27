#include <m3ds/nodes/2d/Sprite2D.hpp>

namespace M3DS {
    Sprite2D::Sprite2D(Texture texture) noexcept
        : texture(std::move(texture))
    {}

    Error Sprite2D::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node2D::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(frame) ||
            !file.write(centre)
        )
            return Error::file_write_fail;

        return texture.serialise(file);
    }

    Error Sprite2D::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Node2D::deserialise(file); error != Error::none)
            return error;

        if (
            !file.read(frame) ||
            !file.read(centre)
        )
            return Error::file_read_fail;

        return texture.deserialise(file);
    }

    void Sprite2D::draw(RenderTarget2D& target) {
        if (texture) {
            const std::uint32_t frameCount = texture.getFrameCount();
            if (frame >= frameCount)
                frame %= frameCount;

            target.drawTextureFrame(
                texture,
                getGlobalTransform(),
                frame,
                centre
            );
        }

        Node2D::draw(target);
    }

    REGISTER_NO_METHODS(Sprite2D);

    REGISTER_MEMBERS(
        Sprite2D,
        // MEMBER(texture),
        MEMBER(frame),
        MEMBER(centre)
    );
}
