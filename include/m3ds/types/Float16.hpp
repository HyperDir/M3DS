#pragma once

#include <bit>
#include <cstdint>
#include <stdfloat>

namespace M3DS {
#ifdef __STDCPP_FLOAT16_T__
    using float16 = std::float16_t;
#else
    class float16 {
        std::uint16_t mValue {};
    public:
        constexpr float16() noexcept = default;
        constexpr float16(const float16&) noexcept = default;

        explicit constexpr float16(std::uint16_t value) noexcept;

        explicit constexpr operator float() const noexcept;
    };
}

template <>
struct std::is_floating_point<M3DS::float16> : std::true_type {};

namespace M3DS {
    constexpr float16::float16(const std::uint16_t value) noexcept : mValue(value) {}

    constexpr float16::operator float() const noexcept {
        // https://gist.github.com/milhidaka/95863906fe828198f47991c813dbe233
        const std::uint32_t sign = static_cast<std::uint32_t>(mValue) >> 15;
        std::uint32_t exponent = (static_cast<std::uint32_t>(mValue) >> 10) & 0x1f;
        std::uint32_t mantissa = static_cast<std::uint32_t>(mValue) & 0x3ff;

        if (exponent == 0) {
            if (mantissa == 0) {
                return std::bit_cast<float>(sign << 31);
            }

            exponent = 127 - 14;
            while ((mantissa & (1 << 10)) == 0) {
                --exponent;
                mantissa <<= 1;
            }

            mantissa &= 0x3FF;
            return std::bit_cast<float>((sign << 31) | (exponent << 23) | (mantissa << 13));
        }
        if (exponent == 0x1F) {
            return std::bit_cast<float>((sign << 31) | (0xFF << 23) | (mantissa << 13));
        }
        return std::bit_cast<float>((sign << 31) | ((exponent + (127-15)) << 23) | (mantissa << 13));
    }
#endif

#ifdef __STDCPP_BFLOAT16_T__
    using bfloat16 = std::bfloat16_t;
#else
    class bfloat16 {
        std::uint16_t value {};
    public:
        explicit constexpr operator float() const noexcept;
    };

    constexpr bfloat16::operator float() const noexcept {
        return std::bit_cast<float>(static_cast<std::uint32_t>(value) << 16);
    }
#endif
}
