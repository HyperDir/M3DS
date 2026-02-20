#include <m3ds/nodes/2d/Sprite2D.hpp>

namespace M3DS {
    Sprite2D::Sprite2D(SpriteSheet sheet) noexcept
        : spritesheet(std::move(sheet))
    {}

    Failure Sprite2D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(frame) ||
            !serialiser.write(centre)
        )
            return Failure{ ErrorCode::file_write_fail };

        return spritesheet.serialise(serialiser);
    }

    Failure Sprite2D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(frame) ||
            !deserialiser.read(centre)
        )
            return Failure{ ErrorCode::file_read_fail };

        return spritesheet.deserialise(deserialiser);
    }

    void Sprite2D::draw(RenderTarget2D& target) {
        if (spritesheet) {
            const std::uint32_t frameCount = spritesheet.getFrameCount();
            if (frame >= frameCount)
                frame %= frameCount;

            target.drawTextureFrame(
                spritesheet,
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
