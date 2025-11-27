#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    enum class ScrollMode : std::uint8_t {
        none = 0,
        vertical = 1,
        horizontal = 1 << 1,
        both = vertical | horizontal
    };

    [[nodiscard]] constexpr bool operator&(ScrollMode lhs, ScrollMode rhs) noexcept;

    class ScrollContainer : public Container {
        M_CLASS(ScrollContainer, Container)
    public:
        [[nodiscard]] ScrollContainer() noexcept;
    protected:
        void input(Input::InputFrame& inputFrame) override;
        void resize() noexcept override;

        void setScrollMode(ScrollMode to) noexcept;

        void updateMinSize() noexcept override;
    private:
        ScrollMode mScrollMode = ScrollMode::vertical;

        Vector2 mScrollPosition {};
    };

    constexpr bool operator&(const ScrollMode lhs, const ScrollMode rhs) noexcept {
        return std::to_underlying(lhs) & std::to_underlying(rhs);
    }
}

