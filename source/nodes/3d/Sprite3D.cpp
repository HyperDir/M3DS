#include <m3ds/nodes/3d/Sprite3D.hpp>

namespace M3DS {
    Sprite3D::Sprite3D(SpriteSheet sheet) noexcept
        : spritesheet(std::move(sheet))
    {}

    void Sprite3D::draw(RenderTarget3D& target) {
        if (spritesheet) {
            const std::uint32_t frameCount = spritesheet.getFrameCount();
            if (frame >= frameCount)
                frame %= frameCount;

            target.drawSprite(
                spritesheet,
                getGlobalTransform(),
                frame,
                pixelSize,
                cullBack,
                billboard
            );
        }

        Node3D::draw(target);
    }

    Failure Sprite3D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(frame) ||
            !serialiser.write(billboard)
        )
            return Failure{ ErrorCode::file_write_fail };

        return spritesheet.serialise(serialiser);
    }

    Failure Sprite3D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(frame) ||
            !deserialiser.read(billboard)
        )
            return Failure{ ErrorCode::file_write_fail };

        return spritesheet.deserialise(deserialiser);
    }

    REGISTER_NO_METHODS(Sprite3D);

    REGISTER_MEMBERS(
        Sprite3D,

        MEMBER(spritesheet),
        MEMBER(frame),
        MEMBER(pixelSize),
        MEMBER(billboard)
    );
}
