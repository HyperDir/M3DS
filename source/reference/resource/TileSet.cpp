#include <m3ds/reference/resource/TileSet.hpp>

namespace M3DS {
    Failure TileSet::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = Resource::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(static_cast<std::uint16_t>(tiles.size())) ||
            !serialiser.write(std::span{tiles})
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure TileSet::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = Resource::deserialise(deserialiser))
            return failure;

        std::uint16_t tileCount;
        if (!deserialiser.read(tileCount))
            return Failure{ ErrorCode::file_read_fail };

        tiles.resize(tileCount);
        if (!deserialiser.read(std::span{tiles}))
            return Failure{ ErrorCode::file_read_fail };

        return Success;
    }

    REGISTER_NO_METHODS(TileSet);
    REGISTER_NO_MEMBERS(TileSet);
}
