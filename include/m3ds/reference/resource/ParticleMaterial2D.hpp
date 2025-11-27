#pragma once

#include <m3ds/reference/Resource.hpp>
#include <m3ds/utils/Texture.hpp>
#include <m3ds/utils/Units.hpp>

#include <random>

namespace M3DS {
    template <typename T>
    struct Range {
        T min {};
        T max = min;

        [[nodiscard]] constexpr T operator()(std::uniform_random_bit_generator auto& generator) const noexcept {
            if (min == max) return min;
            if constexpr (std::is_floating_point_v<T>) {
                return std::uniform_real_distribution<T>{ min, max }(generator);
            } else if constexpr (std::is_integral_v<T>) {
                return std::uniform_int_distribution<T>{ min, max }(generator);
            }
            return {};
        }
    };

    template <typename T>
    struct Range<Vec2<T>> {
        Vec2<T> min {};
        Vec2<T> max = min;

        [[nodiscard]] Vec2<T> operator()(std::uniform_random_bit_generator auto& generator) const {
            if (min == max) return min;
            if constexpr (std::is_floating_point_v<T>) {
                return {
                    std::uniform_real_distribution<T>{ min.x, max.x }(generator),
                    std::uniform_real_distribution<T>{ min.y, max.y }(generator),
                };
            } else if constexpr (std::is_integral_v<T>) {
                return {
                    std::uniform_int_distribution<T>{ min.x, max.x }(generator),
                    std::uniform_int_distribution<T>{ min.y, max.y }(generator),
                };
            }
            return {};
        }
    };

    class ParticleMaterial2D : public Resource {
        M_CLASS(ParticleMaterial2D, Resource)
    public:
        Texture texture {};
        Range<std::uint16_t> frame {};

        std::uint32_t count { 125 };
        Seconds<float> duration { 5 };

        Range< Radians<float> > startAngle { 0, 2.f * std::numbers::pi_v<float> };

        Range< PixelsPerSecond<float> > speed { 125 };
        Range< RadiansPerSecond<float> > angularSpeed { std::numbers::pi_v<float> };
        Range< PixelsPerSecond<float> > orbitSpeed { 0 };

        PixelsPerSecondSquared<float> linearAcceleration { 0 };
        PixelsPerSecondSquared<float> tangentialAcceleration { 0 };
        RadiansPerSecondSquared<float> angularAcceleration { 0 };
        PixelsPerSecondSquared<float> orbitAcceleration { 0 };

        Range< Vector2 > scale { Vector2{2} };

        Vector2 globalAcceleration { 0, 98 };
    };
}
