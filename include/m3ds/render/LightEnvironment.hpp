#pragma once

#include <array>

#include <m3ds/render/Light.hpp>

namespace M3DS {
    class LightEnv {
        C3D_LightEnv mLightEnv {};

        bool mToonShaded {};
    public:
        std::array<Light, 8> mLights {};
        
        explicit LightEnv(bool toonShaded = true) noexcept;

        void bind() noexcept;

        [[nodiscard]] C3D_LightEnv* get() noexcept;
        [[nodiscard]] const C3D_LightEnv* get() const noexcept;

        [[nodiscard]] bool isToonShaded() const noexcept;

        void material(const C3D_Material& material);
    };
}
