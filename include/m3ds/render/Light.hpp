#pragma once

#ifdef __3DS__
#include <citro3d.h>
#endif
#include <m3ds/spatial/Vector3.hpp>

namespace M3DS {
    class Light {
        friend class LightEnv;
#ifdef __3DS__
        C3D_Light internal {};
#elifdef M3DS_SFML
        Vector3 mColour {};
        Vector3 mPosition {};
#endif
    public:
#ifdef __3DS__
        void setColour(const float r, const float g, const float b) noexcept {
            if (r != internal.diffuse[2] || g != internal.diffuse[1] || b != internal.diffuse[0]) {
                C3D_LightColor(&internal, r, g, b);
            }
        }
        void setPosition(const Vector3& to) noexcept {
            const Vec3 fixedPosition {
                static_cast<std::uint16_t>(f32tof16(to.x)),
                static_cast<std::uint16_t>(f32tof16(to.y)),
                static_cast<std::uint16_t>(f32tof16(to.z))
            };
            if (
                internal.conf.position[0] != fixedPosition.x ||
                internal.conf.position[1] != fixedPosition.y ||
                internal.conf.position[2] != fixedPosition.z
            ) {
                // Enable positional light
                internal.conf.config |= 1;

                internal.conf.position[0] = fixedPosition.x;
                internal.conf.position[1] = fixedPosition.y;
                internal.conf.position[2] = fixedPosition.z;
                internal.flags |= C3DF_Light_Dirty;
            }
        }
#elif M3DS_SFML
        void setColour(const Vector3& to) noexcept {
            mColour = to;
        }

        void setPosition(const Vector3& to) noexcept {
            mPosition = to;
        }

        // [[nodiscard]] const Vector3& getColour() const noexcept {
        //     return mColour;
        // }
        // [[nodiscard]] const Vector3& getPosition() const noexcept {
        //     return mPosition;
        // }
#endif
        // Light() noexcept = default;
        //
        // Light(const Light&) = delete;
        // Light(Light&&) = delete;
        //
        // Light& operator=(const Light&) = delete;
        // Light& operator=(Light&&) = delete;

        // void shadowEnable() noexcept {
        //     C3D_LightShadowEnable(&light, true);
        // }
        // void shadowDisable() noexcept {
        //     C3D_LightShadowEnable(&light, false);
        // }
    };
}
