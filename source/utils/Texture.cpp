#include <m3ds/utils/Texture.hpp>

#include <memory>
#include <fstream>

#ifdef M3DS_SFML
#include <SFML/Graphics/Image.hpp>
#endif

#include <m3ds/utils/Path.hpp>

namespace M3DS {
    Texture::operator bool() const noexcept {
        return static_cast<bool>(mRawImage);
    }

    bool Texture::operator==(const Texture& other) const noexcept {
        return mRawImage == other.mRawImage && mFrames == other.mFrames;
    }

    Error Texture::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = M3DS::serialise(getPath(), file); error != Error::none)
            return error;

        if (mFrames.x > 256 || mFrames.y > 256)
            return Error::file_invalid_data;
        if (!file.write(getFrameSize()))
            return Error::file_write_fail;

        return Error::none;
    }

    Error Texture::deserialise(BinaryInFileAccessor file) noexcept {
        std::filesystem::path path {};
        if (const Error error = M3DS::deserialise(path, file); error != Error::none)
            return error;

        Vec2<std::uint32_t> frameCount;
        if (!file.read(frameCount))
            return Error::file_read_fail;

        if (frameCount.x > 256 || frameCount.y > 256)
            return Error::file_invalid_data;

        if (std::expected exp = SharedRawImage::load(path)) {
            mRawImage = std::move(exp.value());
            setFrames(frameCount);
            return Error::none;
        } else {
            return exp.error();
        }
    }

    const std::filesystem::path& Texture::getPath() const noexcept {
        return mRawImage.getPath();
    }

    const Vec2<std::uint32_t>& Texture::getFrames() const noexcept {
        return mFrames;
    }

    const Vec2<std::uint32_t>& Texture::getFrameSize() const noexcept {
        return mFrameSize;
    }

    Texture::Texture(
        SharedRawImage rawImage,
        const Vec2<std::uint32_t> frames
    ) noexcept
        : mRawImage(std::move(rawImage))
    {
        setFrames(frames);
    }

    std::expected<Texture, Error> Texture::load(
        const std::filesystem::path& path,
        const Vec2<std::uint32_t> frames
    ) {
        std::expected<SharedRawImage, Error> exp = SharedRawImage::load(path);
        if (!exp.has_value())
            return std::unexpected{ exp.error() };

        return Texture { std::move(exp.value()), frames };
    }

    std::expected<Texture, Error> Texture::load(
        const std::span<const std::byte> data,
        const Vec2<std::uint32_t> frames
    ) {
        std::expected<SharedRawImage, Error> exp = SharedRawImage::load(data);
        if (!exp.has_value())
            return std::unexpected{ exp.error() };

        return Texture { std::move(exp.value()), frames };
    }

    void Texture::setFrames(const Vec2<std::uint32_t>& frames) noexcept {
        mFrames = frames;

        mFrameSize = static_cast<Vec2<std::uint32_t>>(mRawImage.getSize()) / mFrames;
        mFrameUvSize = 1.f / static_cast<Vector2>(mFrames);
    }

    UVs Texture::getFrame(const Vec2<std::uint32_t> frame) const noexcept {
        const UVs& rawUvs = mRawImage.getUVs();

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

    UVs Texture::getFrame(const std::uint32_t frame) const noexcept {
#ifdef __3DS__
        const UVs& rawUvs = mRawImage.getUVs();
#elifdef M3DS_SFML
        static constexpr UVs rawUvs {0, 1, 0, 1};
#endif

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

    std::uint32_t Texture::getFrameCount() const noexcept {
        return static_cast<std::uint32_t>(mFrames.x * mFrames.y);
    }
}
