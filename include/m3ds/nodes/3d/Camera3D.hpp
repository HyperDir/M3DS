#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>

namespace M3DS {
    class Camera3D : public Node3D {
        M_CLASS(Camera3D, Node3D)
    public:
        void enable() noexcept;
        void disable() noexcept;

        [[nodiscard]] bool isActive() const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        bool mQueuedActive {};
    };
}
