#pragma once

#include <m3ds/nodes/2d/Node2D.hpp>

namespace M3DS {
    class Camera2D : public Node2D {
        M_CLASS(Camera2D, Node2D)
    public:
        void enable() noexcept;
        void disable() noexcept;

        [[nodiscard]] bool isActive() const noexcept;
    protected:
        // void draw(RenderTarget2D& target) noexcept override;
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        bool mQueuedActive {};
    };
}
