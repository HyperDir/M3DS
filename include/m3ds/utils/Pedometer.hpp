#pragma once

#include <cstdint>
#include <cstring>

namespace M3DS {
    class Pedometer {
        inline static size_t count {};
        std::uint32_t initialCount {};
    public:
        Pedometer();
        ~Pedometer();

        [[nodiscard]] std::uint32_t getStepCount() const;
        void resetStepCount();
    };
}
