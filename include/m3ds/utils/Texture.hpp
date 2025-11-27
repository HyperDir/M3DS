#pragma once

#include <expected>
#include <filesystem>

#include <m3ds/utils/Debug.hpp>
#include <m3ds/utils/RawImage.hpp>

#include "BinaryFile.hpp"

namespace M3DS {
    class Texture {
    public:
        Texture() = default;

        explicit Texture(
            SharedRawImage rawImage,
            Vec2<std::uint32_t> frames = { 1, 1 }
        ) noexcept;

        static std::expected<Texture, Error> load(
            const std::filesystem::path& path,
            Vec2<std::uint32_t> frames = { 1, 1 }
        );

        static std::expected<Texture, Error> load(
            std::span<const std::byte> data,
            Vec2<std::uint32_t> frames = { 1, 1 }
        );

        void setFrames(const Vec2<std::uint32_t>& frames) noexcept;

        [[nodiscard]] const std::filesystem::path& getPath() const noexcept;

        [[nodiscard]] const Vec2<std::uint32_t>& getFrames() const noexcept;
        [[nodiscard]] const Vec2<std::uint32_t>& getFrameSize() const noexcept;

        [[nodiscard]] UVs getFrame(Vec2<std::uint32_t> frame) const noexcept;
        [[nodiscard]] UVs getFrame(std::uint32_t frame) const noexcept;

        [[nodiscard]] std::uint32_t getFrameCount() const noexcept;

        [[nodiscard]] explicit operator bool() const noexcept;
        [[nodiscard]] bool operator==(const Texture& other) const noexcept;

        [[nodiscard]] Error serialise(BinaryOutFileAccessor file) const noexcept;
        [[nodiscard]] Error deserialise(BinaryInFileAccessor file) noexcept;
    private:
        friend class RenderTarget2D;
        friend class RenderTarget3D;

        SharedRawImage mRawImage {};
        Vec2<std::uint32_t> mFrames { 1, 1 };
        Vec2<std::uint32_t> mFrameSize {};
        Vector2 mFrameUvSize {};
    };
}
