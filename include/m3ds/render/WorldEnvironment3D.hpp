#pragma once

#include <m3ds/render/Texture.hpp>

namespace M3DS {
    struct WorldEnvironment3D {
        static Texture defaultSkyboxTexture;

        Texture skyboxTexture = defaultSkyboxTexture;
    };
}
