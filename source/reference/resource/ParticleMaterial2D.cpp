#include <m3ds/reference/resource/ParticleMaterial2D.hpp>

namespace M3DS {
    Failure ParticleMaterial2D::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = texture.serialise(serialiser))
            return failure;

        if (
            !serialiser.write(frame) ||
            !serialiser.write(count) ||
            !serialiser.write(duration) ||
            !serialiser.write(startAngle) ||
            !serialiser.write(speed) ||
            !serialiser.write(angularSpeed) ||
            !serialiser.write(orbitSpeed) ||
            !serialiser.write(linearAcceleration) ||
            !serialiser.write(tangentialAcceleration) ||
            !serialiser.write(angularAcceleration) ||
            !serialiser.write(orbitAcceleration) ||
            !serialiser.write(scale) ||
            !serialiser.write(globalAcceleration)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure ParticleMaterial2D::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = texture.deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(frame) ||
            !deserialiser.read(count) ||
            !deserialiser.read(duration) ||
            !deserialiser.read(startAngle) ||
            !deserialiser.read(speed) ||
            !deserialiser.read(angularSpeed) ||
            !deserialiser.read(orbitSpeed) ||
            !deserialiser.read(linearAcceleration) ||
            !deserialiser.read(tangentialAcceleration) ||
            !deserialiser.read(angularAcceleration) ||
            !deserialiser.read(orbitAcceleration) ||
            !deserialiser.read(scale) ||
            !deserialiser.read(globalAcceleration)
        )
            return Failure{ ErrorCode::file_read_fail };

        return Success;
    }

    REGISTER_NO_METHODS(ParticleMaterial2D);
    REGISTER_NO_MEMBERS(ParticleMaterial2D);
}
