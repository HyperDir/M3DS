#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>

namespace M3DS {
    class Light3D : public Node3D {
        M_CLASS(Light3D, Node3D)
    public:
        Vector3 colour {1.f, 1.f, 1.f};

        bool enable() noexcept;
        void disable() noexcept;

        [[nodiscard]] bool isActive() const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        bool mQueuedActive {};
    };
}
