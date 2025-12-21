#pragma once

#include "../Spatial/Units.hpp"

namespace SPhys {
    struct PhysicsEnvironment2D {
        Pixels<float> chunkSize = 4096.f;
        Pixels<float> groundBias = 200.f;
    };
}
