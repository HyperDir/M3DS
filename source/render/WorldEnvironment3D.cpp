#include <m3ds/render/WorldEnvironment3D.hpp>

namespace M3DS {
    constexpr unsigned char skyboxTextureData[] {
#embed <gfx/SkyBox.png.t3x>
    };

    Texture WorldEnvironment3D::defaultSkyboxTexture = Texture::load(std::span{skyboxTextureData, sizeof(skyboxTextureData)}).value();
}
