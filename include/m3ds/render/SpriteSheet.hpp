#pragma once

#include <expected>
#include <filesystem>

#include <m3ds/utils/Debug.hpp>
#include <m3ds/render/Texture.hpp>

#include <m3ds/utils/BinaryFile.hpp>

namespace M3DS {
    class SpriteSheet {
    public:
        SpriteSheet() = default;

        explicit SpriteSheet(
            Texture texture,
            Vec2<std::uint32_t> frames = { 1, 1 }
        ) noexcept;

        static std::expected<SpriteSheet, Failure> load(
            const std::filesystem::path& path,
            Vec2<std::uint32_t> frames = { 1, 1 }
        );

        static std::expected<SpriteSheet, Failure> load(
            std::span<const unsigned char> data,
            Vec2<std::uint32_t> frames = { 1, 1 }
        );

        void setFrames(const Vec2<std::uint32_t>& frames) noexcept;

        [[nodiscard]] PathView getPath() const noexcept;

        [[nodiscard]] const Vec2<std::uint32_t>& getFrames() const noexcept;
        [[nodiscard]] const Vector2& getFrameSize() const noexcept;

        [[nodiscard]] UVs getFrame(Vec2<std::uint32_t> frame) const noexcept;
        [[nodiscard]] UVs getFrame(std::uint32_t frame) const noexcept;

        [[nodiscard]] std::uint32_t getFrameCount() const noexcept;

        [[nodiscard]] explicit operator bool() const noexcept;
        [[nodiscard]] bool operator==(const SpriteSheet& other) const noexcept;

        [[nodiscard]] Failure serialise(BinaryOutFileAccessor file) const noexcept;
        [[nodiscard]] Failure deserialise(BinaryInFileAccessor file) noexcept;

        [[nodiscard]] Texture& getTexture() noexcept;
        [[nodiscard]] const Texture& getTexture() const noexcept;
        [[nodiscard]] C3D_Tex* getNative() const noexcept;
    private:
        friend class RenderTarget2D;
        friend class RenderTarget3D;

        Texture mTexture {};
        Pixels<Vec2<std::uint32_t>> mFrames { 1, 1 };
        Pixels<Vector2> mFrameSize {};
        Vector2 mFrameUvSize {};
    };
}
