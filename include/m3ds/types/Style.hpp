#pragma once

#include <m3ds/types/Colour.hpp>
#include <m3ds/types/Margin.hpp>
#include <m3ds/utils/Texture.hpp>

#include "m3ds/reference/Resource.hpp"

namespace M3DS {
    template <typename T>
    concept StyleInterface = requires(T t) {
        std::is_default_constructible_v<T>;
        { t == t } -> std::same_as<bool>;
        { t.getMinSize() } -> std::same_as<Vector2>;
        { t.contentMargin } -> std::same_as<Margin&>;
        { t.serialise(std::declval<BinaryOutFileAccessor>()) } -> std::same_as<Error>;
        { t.deserialise(std::declval<BinaryInFileAccessor>()) } -> std::same_as<Error>;
    };

    struct BoxStyle {
        Colour colour { Colours::dark_grey };
        Colour centreColour { colour };
        float cornerRadius = 6;
        int cornerDetail = 4;

        Margin contentMargin { 4, 4, 4, 4 };

        constexpr bool operator==(const BoxStyle&) const noexcept = default;

        [[nodiscard]] constexpr Vector2 getMinSize() const noexcept {
            return { 2 * cornerRadius, 2 * cornerRadius };
        }

        [[nodiscard]] Error serialise(BinaryOutFileAccessor file) const noexcept {
            if (!file.write(*this))
                return Error::file_write_fail;
            return Error::none;
        }

        [[nodiscard]] Error deserialise(BinaryInFileAccessor file) noexcept {
            if (!file.read(*this))
                return Error::file_read_fail;
            return Error::none;
        }
    };

    struct TextureStyle {
        Texture texture {};
        std::uint16_t frame {};

        Margin contentMargin { 4, 4, 4, 4 };

        bool operator==(const TextureStyle&) const noexcept = default;

        [[nodiscard]] constexpr Vector2 getMinSize() const noexcept {
            return Vector2{ texture.getFrameSize() };
        }

        [[nodiscard]] Error serialise(BinaryOutFileAccessor file) const noexcept {
            if (const Error error = texture.serialise(file); error != Error::none)
                return error;

            if (!file.write(frame) || !file.write(contentMargin))
                return Error::file_write_fail;

            return Error::none;
        }

        [[nodiscard]] Error deserialise(BinaryInFileAccessor file) noexcept {
            if (const Error error = texture.deserialise(file); error != Error::none)
                return error;

            if (!file.read(frame) || !file.read(contentMargin))
                return Error::file_read_fail;

            return Error::none;
        }
    };

    static_assert(StyleInterface<BoxStyle>);
    static_assert(StyleInterface<TextureStyle>);

    using Style = std::variant<BoxStyle, TextureStyle>;

    [[nodiscard]] inline Error serialise(const Style& style, BinaryOutFileAccessor file) noexcept {
        const std::uint8_t idx = static_cast<std::uint8_t>(style.index());

        if (!file.write(idx))
            return Error::file_write_fail;

        return style.visit(
            [&](const auto& s) -> Error {
                return s.serialise(file);
            }
        );
    }

    [[nodiscard]] inline Error deserialise(Style& style, BinaryInFileAccessor file) {
        std::uint8_t idx;
        if (!file.read(idx))
            return Error::file_read_fail;

        style = variantFromIndex<Style>(idx);

        return style.visit(
            [&](auto& s) -> Error {
                return s.deserialise(file);
            }
        );
    }

    [[nodiscard]] constexpr Vector2 getMinSize(const Style& style) noexcept {
        return style.visit([](const auto& s) -> Vector2 {
            return s.getMinSize();
        });
    }

    [[nodiscard]] constexpr const Margin& getContentMargin(const Style& style) noexcept {
        return style.visit([](const auto& s) -> const Margin& {
            return s.contentMargin;
        });
    }
}
