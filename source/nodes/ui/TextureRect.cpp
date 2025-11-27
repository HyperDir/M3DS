#include <m3ds/nodes/ui/TextureRect.hpp>

namespace M3DS {
    TextureRect::TextureRect(Texture texture) noexcept
        : mTexture(std::move(texture))
    {}

    void TextureRect::setTexture(Texture texture) noexcept {
        mTexture = std::move(texture);

        queueResize();
    }

    const Texture& TextureRect::getTexture() const noexcept {
        return mTexture;
    }

    Error TextureRect::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = UINode::serialise(file); error != Error::none)
            return error;

        if (!file.write(frame) || !file.write(centre))
            return Error::file_write_fail;

        return mTexture.serialise(file);
    }

    Error TextureRect::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = UINode::deserialise(file); error != Error::none)
            return error;

        if (!file.read(frame) || !file.read(centre))
            return Error::file_read_fail;

        return mTexture.deserialise(file);
    }

    void TextureRect::draw(RenderTarget2D& target) {
        if (mTexture) {
            const std::uint32_t frameCount = mTexture.getFrameCount();
            if (frame >= frameCount)
                frame %= frameCount;

            Transform2D xform = getGlobalTransform();
            xform.scale *= getSize() / Vector2{mTexture.getFrameSize()};

            target.drawTextureFrame(mTexture, xform, frame, centre);
        }

        UINode::draw(target);
    }

    void TextureRect::updateMinSize() noexcept {
        UINode::updateMinSize();

        mInternalMinSize = Vector2{mTexture.getFrameSize()};
    }

    REGISTER_NO_METHODS(TextureRect);

    REGISTER_MEMBERS(
        TextureRect,

        MEMBER(frame),
        MEMBER(centre)
    );
}
