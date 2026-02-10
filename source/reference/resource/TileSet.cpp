#include <m3ds/reference/resource/TileSet.hpp>

namespace M3DS {
    Failure TileSet::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = Resource::serialise(file))
            return failure;

        if (
            !file.write(static_cast<std::uint16_t>(tiles.size())) ||
            !file.write(std::span{tiles})
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure TileSet::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Failure failure = Resource::deserialise(file))
            return failure;

        std::uint16_t tileCount;
        if (!file.read(tileCount))
            return Failure{ ErrorCode::file_read_fail };

        tiles.resize(tileCount);
        if (!file.read(std::span{tiles}))
            return Failure{ ErrorCode::file_read_fail };

        return Success;
    }

    REGISTER_NO_METHODS(TileSet);
    REGISTER_NO_MEMBERS(TileSet);
}
