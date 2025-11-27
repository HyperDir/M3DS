#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/types/Signal.hpp>

namespace M3DS {
    class Timer : public Node {
        M_CLASS(Timer, Node)
    public:
        Seconds<float> duration = 10.f;
        bool oneShot = true;

        Signal timeout {};

        void start() noexcept;
        void restart() noexcept;

        void pause() noexcept;
        void stop() noexcept;

        void resume() noexcept;

        [[nodiscard]] bool isActive() const noexcept;
        [[nodiscard]] Seconds<float> getElapsed() const noexcept;
        [[nodiscard]] Seconds<float> getTimeLeft() const noexcept;
    protected:
        void update(Seconds<float> delta) override;
    private:
        bool mActive {};
        Seconds<float> mElapsed {};
    };
}

