#pragma once

#include <m3ds/types/Colour.hpp>
#include <m3ds/types/Margin.hpp>
#include <m3ds/render/SpriteSheet.hpp>

#include "m3ds/reference/Resource.hpp"
#include "m3ds/render/Mesh2D.hpp"

namespace M3DS {
    template <typename T>
    concept StyleInterface = requires(T t) {
        std::is_default_constructible_v<T>;
        { t == t } -> std::same_as<bool>;
        { t.getMinSize() } -> std::same_as<Vector2>;
        { t.contentMargin } -> std::same_as<Margin&>;
        { t.serialise(std::declval<BinaryOutFileAccessor>()) } -> std::same_as<Failure>;
        { t.deserialise(std::declval<BinaryInFileAccessor>()) } -> std::same_as<Failure>;
    };

    struct BoxStyle {
        Colour colour { Colours::dark_grey };
        Margin contentMargin { 8, 8, 8, 8 };

        Texture texture = stretchPanelTexture;

        [[nodiscard]] Vector2 getMinSize() const noexcept;

        [[nodiscard]] bool operator==(const BoxStyle&) const noexcept = default;

        [[nodiscard]] Failure serialise(BinaryOutFileAccessor) const noexcept;
        [[nodiscard]] Failure deserialise(BinaryInFileAccessor) noexcept;
    private:
        static const Texture stretchPanelTexture;
    };

    struct TextureStyle {
        SpriteSheet texture {};
        std::uint16_t frame {};

        Margin contentMargin { 4, 4, 4, 4 };

        bool operator==(const TextureStyle&) const noexcept = default;

        [[nodiscard]] constexpr Vector2 getMinSize() const noexcept {
            return Vector2{ texture.getFrameSize() };
        }

        [[nodiscard]] Failure serialise(BinaryOutFileAccessor file) const noexcept {
            if (const Failure failure = texture.serialise(file))
                return failure;

            if (!file.write(frame) || !file.write(contentMargin))
                return Failure{ ErrorCode::file_write_fail };

            return Success;
        }

        [[nodiscard]] Failure deserialise(BinaryInFileAccessor file) noexcept {
            if (const Failure failure = texture.deserialise(file))
                return failure;

            if (!file.read(frame) || !file.read(contentMargin))
                return Failure{ ErrorCode::file_read_fail };

            return Success;
        }
    };

    static_assert(StyleInterface<BoxStyle>);
    static_assert(StyleInterface<TextureStyle>);

    using Style = std::variant<BoxStyle, TextureStyle>;

    [[nodiscard]] inline Failure serialise(const Style& style, BinaryOutFileAccessor file) noexcept {
        const std::uint8_t idx = static_cast<std::uint8_t>(style.index());

        if (!file.write(idx))
            return Failure{ ErrorCode::file_write_fail };

        return style.visit(
            [&](const auto& s) -> Failure {
                return s.serialise(file);
            }
        );
    }

    [[nodiscard]] inline Failure deserialise(Style& style, BinaryInFileAccessor file) {
        std::uint8_t idx;
        if (!file.read(idx))
            return Failure{ ErrorCode::file_read_fail };

        style = variantFromIndex<Style>(idx);

        return style.visit(
            [&](auto& s) -> Failure {
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
