#pragma once

#include <algorithm>
#include <array>
#include <ranges>

#include <m3ds/render/Light.hpp>

#ifdef __3DS__
static constexpr float toon_diffuse(const float x, [[maybe_unused]] float arg) {
    static constexpr float numDiffuseTones = 4;

    static constexpr float factor = numDiffuseTones - 1;
    return std::floor(0.5f + x * factor) / factor;
}
static constexpr float toon_specular(const float x, const float shininess) {
    static constexpr float numSpecularTones = 3;

    static constexpr float factor = numSpecularTones - 1;
    return std::floor(0.5f + std::pow(x, shininess) * factor) / factor;
}

static constexpr C3D_LightLut LightLut_FromArray(const std::array<float, 512>& data) {
    C3D_LightLut lut {};
    for (std::size_t i = 0; i < 256; i ++) {
        float in = data[i], diff = data[i+256];

        u32 val = 0;
        if (in > 0.0f) {
            in *= 0x1000;
            val = in < 0x1000 ? static_cast<u32>(in) : 0xFFF;
        }

        u32 val2 = 0;
        if (diff != 0.0f) {
            if (diff < 0) {
                diff = -diff;
                val2 = 0x800;
            }
            diff *= 0x800;
            val2 |= diff < 0x800 ? static_cast<u32>(diff) : 0x7FF;
        }

        lut.data[i] = val | (val2 << 12);
    }
    return lut;
}

template <bool negative = false>
static constexpr C3D_LightLut LightLut_FromFunc(const C3D_LightLutFunc func, const float param) {
    std::array<float, 512> data {};

    const int min = negative ? (-128) : 0;
    const int max = negative ?   128  : 256;
    for (int i = min; i <= max; ++i) {
        const float x   = static_cast<float>(i)/max;
        const float val = func(x, param);
        const int   idx = negative ? (i & 0xFF) : i;
        if (i < max)
            data[static_cast<std::size_t>(idx)] = val;
        if (i > min)
            data[static_cast<std::size_t>(idx+255)] = val-data[static_cast<std::size_t>(idx-1)];
    }

    return LightLut_FromArray(data);
}
#endif

namespace M3DS {
    class LightEnv {
        C3D_LightEnv mLightEnv {};

        bool mToonShaded {};
    public:
        std::array<Light, 8> mLights {};
        
        explicit LightEnv(const bool toonShaded = true) noexcept : mToonShaded(toonShaded) {
            C3D_LightEnvInit(&mLightEnv);

            if (mToonShaded) {
                static C3D_LightLut lut_Spec = LightLut_FromFunc(toon_diffuse, 0);
                static C3D_LightLut lut_Toon = LightLut_FromFunc(toon_specular, 30);

                C3D_LightEnvLut(get(), GPU_LUT_D0, GPU_LUTINPUT_NH, false, &lut_Spec);
                C3D_LightEnvLut(get(), GPU_LUT_D1, GPU_LUTINPUT_LN, false, &lut_Toon);
            } else {
                static C3D_LightLut lut_Phong = LightLut_FromFunc(std::pow, 30);

                C3D_LightEnvLut(get(), GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lut_Phong);
            }

            for (std::uint8_t i{}; i < static_cast<std::uint8_t>(mLights.size()); ++i) {
                auto& light = mLights[i].internal;

                get()->lights[i] = &light;
                light.flags = C3DF_Light_Enabled | C3DF_Light_Dirty | C3DF_Light_MatDirty;
                light.id = i;
                light.parent = get();

                get()->flags |= C3DF_LightEnv_LCDirty;
            }
        }

        void bind() noexcept {
            C3D_LightEnvBind(&mLightEnv);
        }

        [[nodiscard]] C3D_LightEnv* get() noexcept {
            return &mLightEnv;
        }

        [[nodiscard]] const C3D_LightEnv* get() const noexcept {
            return &mLightEnv;
        }

        [[nodiscard]] constexpr bool isToonShaded() const noexcept {
            return mToonShaded;
        }

        void material(const C3D_Material& material) {
            C3D_LightEnvMaterial(get(), &material);
        }
    };
}
