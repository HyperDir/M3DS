#include <m3ds/reference/resource/ParticleMaterial2D.hpp>

namespace M3DS {
    Failure ParticleMaterial2D::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = texture.serialise(file))
            return failure;

        if (
            !file.write(frame) ||
            !file.write(count) ||
            !file.write(duration) ||
            !file.write(startAngle) ||
            !file.write(speed) ||
            !file.write(angularSpeed) ||
            !file.write(orbitSpeed) ||
            !file.write(linearAcceleration) ||
            !file.write(tangentialAcceleration) ||
            !file.write(angularAcceleration) ||
            !file.write(orbitAcceleration) ||
            !file.write(scale) ||
            !file.write(globalAcceleration)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure ParticleMaterial2D::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = texture.deserialise(file))
            return failure;

        if (
            !file.read(frame) ||
            !file.read(count) ||
            !file.read(duration) ||
            !file.read(startAngle) ||
            !file.read(speed) ||
            !file.read(angularSpeed) ||
            !file.read(orbitSpeed) ||
            !file.read(linearAcceleration) ||
            !file.read(tangentialAcceleration) ||
            !file.read(angularAcceleration) ||
            !file.read(orbitAcceleration) ||
            !file.read(scale) ||
            !file.read(globalAcceleration)
        )
            return Failure{ ErrorCode::file_read_fail };

        return Success;
    }

    REGISTER_NO_METHODS(ParticleMaterial2D);
    REGISTER_NO_MEMBERS(ParticleMaterial2D);
}
