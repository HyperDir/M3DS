#pragma once

#include <array>

#include <m3ds/nodes/CanvasItem.hpp>

namespace M3DS {
    class Focus {
        std::uint8_t value {};
    public:
        [[nodiscard]] constexpr bool get(const std::uint8_t idx) const noexcept { return static_cast<std::uint8_t>(value & (1 << idx)); }
        constexpr void set(const std::uint8_t idx) noexcept { value |= static_cast<std::uint8_t>(1 << idx); }
        constexpr void clear(const std::uint8_t idx) noexcept { value &= static_cast<std::uint8_t>(~(1 << idx)); }

        constexpr explicit operator bool() const noexcept { return value != 0; }
    };

    constexpr std::array focusColours {
        Colours::green,
        Colours::red,
        Colours::cyan,
        Colours::yellow,
        Colours::orange,
        Colours::blue,
        Colours::indigo,
        Colours::violet,
    };

    class UINode : public CanvasItem {
        M_CLASS(UINode, CanvasItem)
    public:
        struct Neighbours {
            NodePath up {};
            NodePath down {};
            NodePath right {};
            NodePath left {};
        } neighbours {};

        [[nodiscard]] bool needsResize() const noexcept;
        void queueResize() noexcept;

        void takeFocus(std::uint8_t idx) noexcept;
        void clearFocus(std::uint8_t idx) noexcept;

        void setSize(const Vector2& to) noexcept;
        const Vector2& getSize() const noexcept;

        void setMinSize(const Vector2& to) noexcept;
        Vector2 getMinSize() const noexcept;

        void setFillSpace(bool to) noexcept;
        [[nodiscard]] bool getFillSpace() const noexcept;

        void setClipContent(bool to) noexcept;
        [[nodiscard]] bool getClipContent() const noexcept;
    protected:
        void update(Seconds<float> delta) override;
        void draw(RenderTarget2D& target) override;

        void shrinkToFit(const Vector2& size) noexcept;

        void afterTreeEnter() override;

        void shrink() noexcept;

        virtual void updateMinSize() noexcept;
        virtual void resize() noexcept;

        bool isOccluded(const Vector2& point) const noexcept;
    private:
        Pixels<Vector2> mSize {};
        Pixels<Vector2> mUserMinSize {};
    protected:
        Pixels<Vector2> mInternalMinSize {};
    private:
        Focus mFocus {};

        bool mFillSpace {};
        bool mNeedsResize {};
        bool mClipContent {};

        void resizeTree() noexcept;
    };
}
