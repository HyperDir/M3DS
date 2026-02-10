#pragma once

#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/types/Error.hpp>

namespace M3DS {
    [[nodiscard]] inline Failure serialise(const std::filesystem::path& path, BinaryOutFileAccessor file) noexcept {
        const std::string pathString = path.string();
        if (pathString.length() > 1024)
            return Failure{ ErrorCode::file_write_fail };

        if (
            !file.write(static_cast<std::uint16_t>(pathString.length())) ||
            !file.write(std::span{pathString})
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    [[nodiscard]] inline Failure deserialise(std::filesystem::path& path, BinaryInFileAccessor file) noexcept {
        std::uint16_t length;
        if (!file.read(length))
            return Failure{ ErrorCode::file_read_fail };

        if (length > 1024)
            return Failure{ ErrorCode::file_invalid_data };

        std::string str {};
        str.resize(length);
        if (!file.read(std::span{str}))
            return Failure{ ErrorCode::file_read_fail };

        path = { std::move(str) };
        return Success;
    }
}
