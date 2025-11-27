#include <m3ds/utils/Pedometer.hpp>

#ifdef __3DS__
#include <3ds.h> // Including just types.h and ptm.h doesn't seem to compile

namespace M3DS {
    Pedometer::Pedometer() {
        if (count == 0) {
            ptmuInit();
        }
        ++count;

        resetStepCount();
    }

    Pedometer::~Pedometer() {
        if (--count == 0) {
            ptmuExit();
        }
    }

    void Pedometer::resetStepCount() {
        PTMU_GetTotalStepCount(&initialCount);
    }

    std::uint32_t Pedometer::getStepCount() const {
        uint32_t newCount {};

        PTMU_GetTotalStepCount(&newCount);

        return newCount - initialCount;
    }
}
#elifdef M3DS_SFML
namespace M3DS {
    Pedometer::Pedometer() = default;
    Pedometer::~Pedometer() = default;

    std::uint32_t Pedometer::getStepCount() const { return 0; }
    void Pedometer::resetStepCount() {}
}
#endif
