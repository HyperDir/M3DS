#pragma once

#include <expected>
#include <filesystem>
#include <memory>

#include <m3ds/types/Error.hpp>

#ifdef __3DS__
#include <citro2d.h>
#elifdef M3DS_SFML
#include <SFML/Graphics/Texture.hpp>
#endif

#include <m3ds/spatial/Vector2.hpp>

namespace M3DS {
    struct UVs {
        float left {};
        float right {};
        float top {};
        float bottom {};
    };

    class SharedRawImage {
    public:
        static std::expected<SharedRawImage, Error> load(const std::filesystem::path& path) noexcept;
        static std::expected<SharedRawImage, Error> load(std::span<const std::byte> data) noexcept;

        SharedRawImage() noexcept = default;
        ~SharedRawImage() noexcept;

        SharedRawImage(const SharedRawImage&) = default;
        SharedRawImage& operator=(const SharedRawImage&) = default;

        SharedRawImage(SharedRawImage&&) noexcept = default;
        SharedRawImage& operator=(SharedRawImage&&) noexcept = default;

#ifdef __3DS__
        [[nodiscard]] C2D_SpriteSheet_s* getNativeHandle() const noexcept;
#elifdef M3DS_SFML
        [[nodiscard]] sf::Texture* getNativeHandle() const noexcept;
#endif

        [[nodiscard]] const std::filesystem::path& getPath() const noexcept;
        [[nodiscard]] const UVs& getUVs() const noexcept;
        [[nodiscard]] const Vec2<std::uint16_t>& getSize() const noexcept;

        [[nodiscard]] explicit operator bool() const;

        [[nodiscard]] bool operator==(const SharedRawImage& other) const noexcept;
    private:
#ifdef __3DS__
        std::shared_ptr<C2D_SpriteSheet_s> mSpriteSheet {};
#elifdef M3DS_SFML
        std::shared_ptr<sf::Texture> mTexture {};
#endif
        std::filesystem::path mPath {};
        UVs mUvs { 0, 1, 0, 1 };
        Vec2<std::uint16_t> mSize {};

#ifdef __3DS__
        explicit SharedRawImage(
            std::shared_ptr<C2D_SpriteSheet_s> spriteSheet,
            std::filesystem::path path = {}
        ) noexcept;
#elifdef M3DS_SFML
        explicit SharedRawImage(
            std::shared_ptr<sf::Texture> texture,
            std::filesystem::path path = {}
        );
#endif

        void setSizeAndUvs() noexcept;
    };
}