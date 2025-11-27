#pragma once

#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    enum class Align : std::uint8_t {
        begin,
        centre,
        end
    };

    class BoxContainer : public Container {
        M_CLASS(BoxContainer, Container)
    public:
        explicit BoxContainer(bool vertical = false) noexcept;

        void setAlign(Align to) noexcept;
        [[nodiscard]] Align getAlign() const noexcept;

        void setSeparation(float to) noexcept;
        [[nodiscard]] float getSeparation() const noexcept;
    protected:
        void updateMinSize() noexcept override;
        void resize() noexcept override;
    private:
        Align mAlign {};
        float mSeparation = 4.f;
        bool mVertical {};
    };
}
