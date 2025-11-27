#pragma once

#include <cstdint>
#include <iostream>

namespace M3DS {
    struct Colour {
        std::uint8_t r {}, g {}, b {}, a {};

        constexpr Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF);
        constexpr explicit Colour(std::uint32_t c = 0xFFFFFFFF);

        explicit constexpr operator std::uint32_t() const;

        constexpr bool operator==(const Colour& other) const noexcept = default;

        template <typename... Args>
        static constexpr Colour mix(const Args&... args) {
            return {
                static_cast<std::uint8_t>((args.r + ...) / sizeof...(Args)),
                static_cast<std::uint8_t>((args.g + ...) / sizeof...(Args)),
                static_cast<std::uint8_t>((args.b + ...) / sizeof...(Args)),
                static_cast<std::uint8_t>((args.a + ...) / sizeof...(Args))
            };
        }
    };

    constexpr Colour::Colour(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {}

    constexpr Colour::Colour(const std::uint32_t c)
        : r(static_cast<std::uint8_t>(c >> 24))
        , g(static_cast<std::uint8_t>(c >> 16))
        , b(static_cast<std::uint8_t>(c >> 8))
        , a(static_cast<std::uint8_t>(c))
    {}

    constexpr Colour::operator std::uint32_t() const {
        return (
            static_cast<std::uint32_t>(a) << 24 |
            static_cast<std::uint32_t>(b) << 16 |
            static_cast<std::uint32_t>(g) << 8  |
            static_cast<std::uint32_t>(r)
        );
    }

    constexpr std::ostream& operator<<(std::ostream& stream, const Colour& colour) {
        return stream
            << '('
            << static_cast<int>(colour.r) << ", "
            << static_cast<int>(colour.g) << ", "
            << static_cast<int>(colour.b) << ", "
            << static_cast<int>(colour.a)
            << ')';
    }

    namespace Colours {
        constexpr Colour white       { 0xFFFFFFFF };
        constexpr Colour black       { 0x000000FF };
        constexpr Colour grey        { 0x808080FF };
        constexpr Colour light_grey  { 0xD3D3D3FF };
        constexpr Colour dark_grey   { 0x5A5A5AFF };
        constexpr Colour red         { 0xFF0000FF };
        constexpr Colour orange      { 0xFFA500FF };
        constexpr Colour yellow      { 0xFFFF00FF };
        constexpr Colour green       { 0x00FF00FF };
        constexpr Colour blue        { 0x0000FFFF };
        constexpr Colour indigo      { 0x4B0082FF };
        constexpr Colour violet      { 0x7F00FFFF };
        constexpr Colour magenta     { 0xFF00FFFF };
        constexpr Colour cyan        { 0x00FFFFFF };
        constexpr Colour turquoise   { 0x00868BFF };
        constexpr Colour pink        { 0xFFC0CBFF };
        constexpr Colour lime        { 0x32CD32FF };
        constexpr Colour sky_blue    { 0x87CEEBFF };
        constexpr Colour transparent { 0x00000000 };
    }
}