#include <m3ds/nodes/ui/TextureRect.hpp>

namespace M3DS {
    TextureRect::TextureRect(SpriteSheet texture) noexcept
        : mTexture(std::move(texture))
    {}

    void TextureRect::setTexture(SpriteSheet texture) noexcept {
        mTexture = std::move(texture);

        queueResize();
    }

    const SpriteSheet& TextureRect::getTexture() const noexcept {
        return mTexture;
    }

    Failure TextureRect::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (!file.write(frame) || !file.write(centre))
            return Failure{ ErrorCode::file_write_fail };

        return mTexture.serialise(file);
    }

    Failure TextureRect::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (!file.read(frame) || !file.read(centre))
            return Failure{ ErrorCode::file_read_fail };

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
