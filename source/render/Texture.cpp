#include <m3ds/render/Texture.hpp>

#include <flat_map>

#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/utils/Memory.hpp>
#include <m3ds/utils/Path.hpp>

#include <tex3ds.h>

#include "../../include/m3ds/types/Failure.hpp"

namespace M3DS {
    struct TextureData {
        C3D_Tex native {};
        Pixels<Vector2> size {};
        UVs uvs {};

        std::filesystem::path path {};

        ~TextureData() noexcept;
    };

    std::unordered_map<PathView, std::weak_ptr<TextureData>> textureRegistry {};

    TextureData::~TextureData() noexcept {
        textureRegistry.erase(path.native());
        C3D_TexDelete(&native);
    }


    void updateData(TextureData& value, Tex3DS_Texture_s* tex) noexcept {
        const Tex3DS_SubTexture* subtex = Tex3DS_GetSubTexture(tex, 0);

        Tex3DS_SubTextureTopLeft(subtex, &value.uvs.left, &value.uvs.top);
        Tex3DS_SubTextureBottomRight(subtex, &value.uvs.right, &value.uvs.bottom);

        value.size.x = static_cast<float>(subtex->width);
        value.size.y = static_cast<float>(subtex->height);
    }


    Texture::Texture(std::shared_ptr<TextureData> data) noexcept
        : mData(std::move(data))
        , mSize(mData->size)
        , mUvs(mData->uvs)
    {}

    // TODO: re-implement Tex3DS functions to remove extra heap allocation?
    std::expected<Texture, Failure> Texture::load(std::filesystem::path path, const bool vram) noexcept {
        if (const auto it = textureRegistry.find(path.native()); it != textureRegistry.end()) {
            if (it->first == path) {
                if (std::shared_ptr<TextureData> data = it->second.lock()) {
                    return Texture { std::move(data) };
                }
            }
        }

        BinaryInFile file { path };
        if (!file)
            return std::unexpected{ Failure{ ErrorCode::file_open_fail } };

        auto textureData = std::make_shared<TextureData>();
        textureData->path = std::move(path);

        Tex3DS_Texture_s* texture = Tex3DS_TextureImportStdio(file.getNativeHandle(), &textureData->native, nullptr, vram);
        file.close();
        if (!texture)
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };

        updateData(*textureData, texture);
        Tex3DS_TextureFree(texture);



        textureRegistry.emplace(
            textureData->path.native(),
            textureData
        );

        return Texture{ std::move(textureData) };
    }

    std::expected<Texture, Failure> Texture::load(const std::span<const unsigned char> data, const bool vram) noexcept {
        auto textureData = std::make_shared<TextureData>();

        Tex3DS_Texture_s* texture = Tex3DS_TextureImport(data.data(), data.size(), &textureData->native, nullptr, vram);
        if (!texture)
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };

        updateData(*textureData, texture);
        Tex3DS_TextureFree(texture);

        return Texture{ std::move(textureData) };
    }

    PathView Texture::getPath() const noexcept {
        if (!mData)
            return {};

        return mData->path.native();
    }

    Texture::operator bool() const noexcept {
        return static_cast<bool>(mData);
    }

    C3D_Tex* Texture::getNative() const noexcept {
        return &mData->native;
    }

    const UVs& Texture::getUvs() const noexcept {
        return mUvs;
    }

    const Pixels<Vector2>& Texture::getSize() const noexcept {
        return mSize;
    }

    Failure Texture::serialise(Serialiser& serialiser) const noexcept {
        return M3DS::serialise(getPath(), serialiser);
    }

    Failure Texture::deserialise(Deserialiser& deserialiser) noexcept {
        std::filesystem::path path {};
        if (const Failure failure = M3DS::deserialise(path, deserialiser))
            return failure;

        if (std::expected exp = load(path)) {
            *this = std::move(exp.value());
            return Success;
        } else {
            return exp.error();
        }
    }
}
