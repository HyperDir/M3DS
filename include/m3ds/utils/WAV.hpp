#pragma once

#include <expected>

#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/utils/Units.hpp>
#include <m3ds/types/Error.hpp>

namespace M3DS {
    class WAV {
    public:
        [[nodiscard]] static std::expected<std::shared_ptr<WAV>, StringError> load(const std::filesystem::path& path) noexcept;

        struct FormatInfo {
            std::uint32_t blockSize {};

            std::uint16_t audioFormat {};
            std::uint16_t channels {};

            std::uint32_t sampleRate {};
            std::uint32_t bytesPerSec {};
            std::uint16_t bytesPerBlock {};
            std::uint16_t bitsPerSample {};
        };

        [[nodiscard]] const FormatInfo& getFormatInfo() const noexcept;

        void seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) noexcept;
        [[nodiscard]] long tell() const noexcept;

        template <TrivialIOType T>
        bool read(T& to) noexcept;

        template <TrivialIOType T>
        bool read(std::span<T> to) noexcept;

        std::streamsize getDataLength() const noexcept;
        Seconds<float> getDuration() const noexcept;
    private:
        BinaryInFile mFile {};
        long mDataOffset {};

        FormatInfo mFmt {};

        [[nodiscard]] explicit WAV(const std::filesystem::path& path) noexcept;
    };

    template <TrivialIOType T>
    bool WAV::read(T& to) noexcept {
        return mFile.read(to);
    }

    template <TrivialIOType T>
    bool WAV::read(const std::span<T> to) noexcept {
        return mFile.read(to);
    }
}
