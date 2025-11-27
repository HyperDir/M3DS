#include <flat_map>
#include <m3ds/utils/RawImage.hpp>

#include <fstream>

#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/utils/Debug.hpp>

namespace M3DS {
#ifdef __3DS__
    static std::flat_map<std::filesystem::path, std::weak_ptr<C2D_SpriteSheet_s>> rawImages {};
#elifdef M3DS_SFML
    static std::flat_map<std::filesystem::path, std::weak_ptr<sf::Texture>> rawImages {};
#endif

#ifdef __3DS__
    std::expected<SharedRawImage, Error> SharedRawImage::load(const std::filesystem::path& path) noexcept {
        if (const auto it = rawImages.find(path); it != rawImages.end()) {
            if (std::shared_ptr<C2D_SpriteSheet_s> texture = it->second.lock()) {
                Debug::log<1>("Raw Image '{}' already loaded! Passing reference...", path);
                return SharedRawImage{std::move(texture), path};
            }
            Debug::warn("Raw Image '{}' Expired, but not removed from registry!", path);
        }

        if(!exists(path)) {
            Debug::err("Invalid Raw Image path: '{}'!", path);
            return std::unexpected{ Error::directory_not_found };
        }

        Debug::log<1>("Loading Raw Image '{}'...", path);

        C2D_SpriteSheet_s* const sheet = C2D_SpriteSheetLoad(path.c_str());
        if (!sheet) {
            Debug::err("Failed to load RawImage {}", path);
            return std::unexpected{ Error::file_invalid_data };
        }

        return SharedRawImage{std::shared_ptr<C2D_SpriteSheet_s>{sheet, &C2D_SpriteSheetFree}, path};
    }

    std::expected<SharedRawImage, Error> SharedRawImage::load(const std::span<const std::byte> data) noexcept {
        C2D_SpriteSheet_s* const sheet = C2D_SpriteSheetLoadFromMem(data.data(), data.size());
        if (!sheet) {
            Debug::err("Failed to load RawImage from {} bytes", data.size());
            return std::unexpected{ Error::file_invalid_data };
        }

        return SharedRawImage{std::shared_ptr<C2D_SpriteSheet_s>{sheet, &C2D_SpriteSheetFree}};
    }

    C2D_SpriteSheet_s* SharedRawImage::getNativeHandle() const noexcept {
        return mSpriteSheet.get();
    }

    SharedRawImage::operator bool() const {
        return static_cast<bool>(mSpriteSheet);
    }

    bool SharedRawImage::operator==(const SharedRawImage& other) const noexcept {
        return mSpriteSheet == other.mSpriteSheet;
    }

    SharedRawImage::SharedRawImage(
        std::shared_ptr<C2D_SpriteSheet_s> spriteSheet,
        std::filesystem::path path
    ) noexcept
        : mSpriteSheet(std::move(spriteSheet))
        , mPath(std::move(path))
    {
        setSizeAndUvs();
    }

    void SharedRawImage::setSizeAndUvs() noexcept {
        const auto im = C2D_SpriteSheetGetImage(mSpriteSheet.get(), 0);

        mSize = { im.subtex->width, im.subtex->height };
        mUvs = {
            im.subtex->left,
            im.subtex->right,
            im.subtex->top,
            im.subtex->bottom
        };
    }
#elifdef M3DS_SFML
    sf::Texture* SharedRawImage::getNativeHandle() const noexcept {
        return mTexture.get();
    }

    SharedRawImage::operator bool() const {
        return static_cast<bool>(mTexture) && mTexture->getSize() != sf::Vector2u{ 0, 0 };
    }

    bool SharedRawImage::operator==(const SharedRawImage& other) const noexcept {
        return mTexture == other.mTexture;
    }

    SharedRawImage::SharedRawImage(std::shared_ptr<sf::Texture> texture, std::filesystem::path path)
        : mTexture(std::move(texture))
        , mPath(std::move(path))
    {
        setSizeAndUvs();
    }

    void SharedRawImage::setSizeAndUvs() noexcept {
        const auto size = mTexture->getSize();
        mSize = { static_cast<std::uint16_t>(size.x), static_cast<std::uint16_t>(size.y) };
        mUvs = {
            0,
            1,
            0,
            1
        };
    }

    std::expected<SharedRawImage, Error> SharedRawImage::load(const std::filesystem::path& path) noexcept {
        if (const auto it = rawImages.find(path); it != rawImages.end()) {
            if (std::shared_ptr<sf::Texture> texture = it->second.lock()) {
                Debug::log<1>("Raw Image '{}' already loaded! Passing reference...", path);
                return SharedRawImage{std::move(texture), path};
            }
            Debug::warn("Raw Image '{}' Expired, but not removed from registry!", path);
        }

        if(!exists(path))
            return std::unexpected{ std::format("Invalid Raw Image path: '{}'!", path) };

        Debug::log<1>("Loading Raw Image '{}'...", path);

        auto sheet = std::make_shared<sf::Texture>();
        if (!sheet->loadFromFile(path))
            return std::unexpected{ std::format("Failed to load RawImage {}", path) };

        return SharedRawImage{std::move(sheet), path};
    }

    std::expected<SharedRawImage, Error> SharedRawImage::load(const std::span<const std::byte> data) noexcept {
        auto sheet = std::make_shared<sf::Texture>();
        if (!sheet->loadFromMemory(data.data(), data.size()))
            return std::unexpected{ std::format("Failed to load RawImage from {} bytes", data.size()) };

        return SharedRawImage{ std::move(sheet) };
    }
#endif

    SharedRawImage::~SharedRawImage() noexcept {
        rawImages.erase(mPath);
    }

    const std::filesystem::path& SharedRawImage::getPath() const noexcept {
        return mPath;
    }

    const UVs& SharedRawImage::getUVs() const noexcept {
        return mUvs;
    }

    const Vec2<std::uint16_t>& SharedRawImage::getSize() const noexcept {
        return mSize;
    }
}
