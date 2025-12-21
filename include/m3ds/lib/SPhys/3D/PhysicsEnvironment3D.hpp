#pragma once

#include "../Spatial/Units.hpp"

namespace SPhys {
    struct PhysicsEnvironment3D {
        Metres<float> chunkSize = 16.f;
        Metres<float> groundBias = .2f;
    };
}
