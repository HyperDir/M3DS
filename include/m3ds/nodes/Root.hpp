#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/utils/Frame.hpp>

namespace M3DS {
    enum class Draw : std::uint8_t {
        none = 0,
        draw_2d = 1,
        draw_3d = 1 << 1,
        draw_all = draw_2d | draw_3d
    };

    constexpr bool operator&(const Draw lhs, const Draw rhs) noexcept {
        return std::to_underlying(lhs) & std::to_underlying(rhs);
    }

    template <typename T>
    concept MainLoopCallable = requires(T t, float delta) {
        { t(delta) } -> std::same_as<void>;
    };

    class Root : public Node {
        M_CLASS(Root, Node)
    public:
        Root() noexcept;

        void treeUpdate(Seconds<float> delta) noexcept;
        void treeDraw(Draw draw = Draw::draw_all) noexcept;
        void treeInput() noexcept;

        std::span<Viewport* const> getViewports() noexcept;
        [[nodiscard]] std::span<const Viewport* const> getViewports() const noexcept;
    protected:
        void notification(Notification notification) override;
    public:
        void exit() noexcept;

        void mainLoop() noexcept;
        void mainLoop(MainLoopCallable auto callable) noexcept;

        void enableUpdate(Node* node);
        void disableUpdate(Node* node);

        void addToFreeQueue(Node* node);
    protected:
        friend class Viewport;

        void addViewport(Viewport* viewport);
        void removeViewport(Viewport* viewport);
    private:
        FrameTimer frameTimer {};
        bool mExit {};

        std::vector<Viewport*> mViewports {};
        std::vector<Node*> mUpdateList {};

        std::queue<Node*> mFreeQueue {};
    };

    void Root::mainLoop(MainLoopCallable auto callable) noexcept {
        while (!mExit) {
            const Frame _ {};
#ifdef __3DS__
            if (aptShouldClose())
                break;
#endif
            DrawEnvironment _ {};

            std::invoke(callable, frameTimer());

            while (!mFreeQueue.empty()) {
                mFreeQueue.front()->free();
                mFreeQueue.pop();
            }
        }
    }
}
