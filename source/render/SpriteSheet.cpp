#include <m3ds/render/SpriteSheet.hpp>

#include <memory>
#include <fstream>

#include <m3ds/utils/Path.hpp>

namespace M3DS {
    SpriteSheet::operator bool() const noexcept {
        return static_cast<bool>(mTexture);
    }

    bool SpriteSheet::operator==(const SpriteSheet& other) const noexcept {
        return mTexture == other.mTexture && mFrames == other.mFrames;
    }

    Failure SpriteSheet::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = M3DS::serialise(getPath(), serialiser))
            return failure;

        if (mFrames.x > 256 || mFrames.y > 256)
            return Failure{ ErrorCode::invalid_data };
        if (!serialiser.write(getFrameSize()))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure SpriteSheet::deserialise(Deserialiser& deserialiser) noexcept {
        std::filesystem::path path {};
        if (const Failure failure = M3DS::deserialise(path, deserialiser))
            return failure;

        Vec2<std::uint32_t> frameCount;
        if (!deserialiser.read(frameCount))
            return Failure{ ErrorCode::file_read_fail };

        if (frameCount.x > 256 || frameCount.y > 256)
            return Failure{ ErrorCode::invalid_data };

        if (std::expected exp = Texture::load(path)) {
            mTexture = std::move(exp.value());
            setFrames(frameCount);
            return Success;
        } else {
            return exp.error();
        }
    }

    PathView SpriteSheet::getPath() const noexcept {
        return mTexture.getPath();
    }

    const Vec2<std::uint32_t>& SpriteSheet::getFrames() const noexcept {
        return mFrames;
    }

    const Pixels<Vector2>& SpriteSheet::getFrameSize() const noexcept {
        return mFrameSize;
    }

    SpriteSheet::SpriteSheet(
        Texture texture,
        const Vec2<std::uint32_t> frames
    ) noexcept
        : mTexture(std::move(texture))
    {
        setFrames(frames);
    }

    std::expected<SpriteSheet, Failure> SpriteSheet::load(
        const std::filesystem::path& path,
        const Vec2<std::uint32_t> frames
    ) {
        std::expected<Texture, Failure> exp = Texture::load(path);
        if (!exp.has_value())
            return std::unexpected{ exp.error() };

        return SpriteSheet { std::move(exp.value()), frames };
    }

    std::expected<SpriteSheet, Failure> SpriteSheet::load(
        const std::span<const unsigned char> data,
        const Vec2<std::uint32_t> frames
    ) {
        std::expected<Texture, Failure> exp = Texture::load(data);
        if (!exp.has_value())
            return std::unexpected{ exp.error() };

        return SpriteSheet { std::move(exp.value()), frames };
    }

    void SpriteSheet::setFrames(const Vec2<std::uint32_t>& frames) noexcept {
        mFrames = frames;

        mFrameSize = mTexture.getSize() / static_cast<Vector2>(mFrames);
        mFrameUvSize = 1.f / static_cast<Vector2>(mFrames);
    }

    UVs SpriteSheet::getFrame(const Vec2<std::uint32_t> frame) const noexcept {
        const UVs& rawUvs = mTexture.getUvs();

        const Vector2 uvDelta = mFrameUvSize * Vector2{
            rawUvs.right - rawUvs.left,
            rawUvs.top - rawUvs.bottom
        };

        const Vector2 textureUvPos = uvDelta * Vector2{
            static_cast<float>(frame.x),
            static_cast<float>(frame.y)
        };

        const float left = rawUvs.left + textureUvPos.x;
        const float top = rawUvs.top - textureUvPos.y;

        return {
            left,
            left + uvDelta.x,
            top,
            top - uvDelta.y
        };
    }

    UVs SpriteSheet::getFrame(const std::uint32_t frame) const noexcept {
        const UVs& rawUvs = mTexture.getUvs();

        const Vector2 uvDelta = mFrameUvSize * Vector2{
            rawUvs.right - rawUvs.left,
            rawUvs.top - rawUvs.bottom
        };

        const Vector2 frameVec {
            std::fmod(static_cast<float>(frame), static_cast<float>(mFrames.x)),
            std::floor(static_cast<float>(frame) / static_cast<float>(mFrames.x))
        };

        const Vector2 textureUvPos = uvDelta * frameVec;

        const float left = rawUvs.left + textureUvPos.x;
        const float top = rawUvs.top - textureUvPos.y;

        return {
            left,
            left + uvDelta.x,
            top,
            top - uvDelta.y
        };
    }

    std::uint32_t SpriteSheet::getFrameCount() const noexcept {
        return mFrames.x * mFrames.y;
    }

    Texture& SpriteSheet::getTexture() noexcept {
        return mTexture;
    }

    const Texture& SpriteSheet::getTexture() const noexcept {
        return mTexture;
    }

    C3D_Tex* SpriteSheet::getNative() const noexcept {
        return mTexture.getNative();
    }
}
