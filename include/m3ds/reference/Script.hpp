#pragma once

#include <m3ds/utils/Units.hpp>
#include <m3ds/render/RenderTarget.hpp>
#include <m3ds/types/InputFrame.hpp>

namespace M3DS {
    class BaseScript {
    public:
        virtual ~BaseScript() = default;

        virtual void ready() {}
        virtual void update([[maybe_unused]] Seconds<float> delta) {}
        virtual void draw([[maybe_unused]] const RenderTarget2D& target) {}
        virtual void draw([[maybe_unused]] const RenderTarget3D& target) {}
        virtual void input([[maybe_unused]] Input::InputFrame& inputFrame) {}
    };

    template <typename T>
    class Script : public BaseScript {
    public:
        using NodeType = T;
    private:
        friend class Node;

        NodeType* mNode {};
    public:
        NodeType* base() { return mNode; }
        const NodeType* base() const { return mNode; }
    };

    template <typename T> concept script_type = std::derived_from<T, BaseScript>;
}
