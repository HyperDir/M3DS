#pragma once

#include <cstdint>
#ifdef __3DS__
#include <3ds/os.h>
#include <3ds/svc.h>
#endif

#include <chrono>

#include <m3ds/utils/Units.hpp>

namespace M3DS {
#ifdef __3DS__
    class FrameTimer {
        std::uint64_t prevTick = svcGetSystemTick();
    public:
        Seconds<float> operator()() noexcept {
            const std::uint64_t curTick = svcGetSystemTick();
            float delta = static_cast<float>(curTick - prevTick) / SYSCLOCK_ARM11;
            prevTick = curTick;
            if (delta > 1)
                delta = 1.0f / 60.f;
            return delta;
        }
    };
#elifdef M3DS_SFML
    class FrameTimer {
         std::chrono::time_point<std::chrono::system_clock> prevTime = std::chrono::high_resolution_clock::now();
    public:
        Seconds<float> operator()() noexcept {
            const auto currentTime = std::chrono::high_resolution_clock::now();
            const float delta = std::chrono::duration<float>(currentTime - prevTime).count();
            prevTime = currentTime;
            return delta;
        }
    };
#endif
}
