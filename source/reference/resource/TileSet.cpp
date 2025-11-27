#include <m3ds/reference/resource/TileSet.hpp>

namespace M3DS {
    Error TileSet::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Resource::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(static_cast<std::uint16_t>(tiles.size())) ||
            !file.write(std::span{tiles})
        )
            return Error::file_write_fail;

        return Error::none;
    }

    Error TileSet::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Resource::deserialise(file); error != Error::none)
            return error;

        std::uint16_t tileCount;
        if (!file.read(tileCount))
            return Error::file_read_fail;

        tiles.resize(tileCount);
        if (!file.read(std::span{tiles}))
            return Error::file_read_fail;

        return Error::none;
    }

    REGISTER_NO_METHODS(TileSet);
    REGISTER_NO_MEMBERS(TileSet);
}
