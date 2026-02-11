#include <m3ds/utils/WAV.hpp>

#include <array>
#include <iostream>

#include <m3ds/utils/Debug.hpp>
#include <m3ds/nodes/AudioPlayer.hpp>

namespace M3DS {
    const WAV::FormatInfo& WAV::getFormatInfo() const noexcept {
        return mFmt;
    }

    void WAV::seek(const long offset, const std::ios_base::seekdir dir) noexcept {
        if (dir == std::ios_base::beg) {
            std::ignore = mFile.seek(offset + mDataOffset, dir);
        } else {
            std::ignore = mFile.seek(offset, dir);
        }
    }

    long WAV::tell() const noexcept {
        return mFile.tell() - mDataOffset;
    }

    std::streamsize WAV::getDataLength() const noexcept {
        return mFile.length();
    }

    Seconds<float> WAV::getDuration() const noexcept {
        return static_cast<float>(getDataLength()) / static_cast<float>(mFmt.bytesPerSec);
    }

    WAV::WAV(const std::filesystem::path& path) noexcept : mFile(path) {}

    std::expected<std::shared_ptr<WAV>, Failure> WAV::load(const std::filesystem::path &path) noexcept {
        struct WAVHeader {
            std::array<char, 4> riffTag {};
            std::uint32_t fileSize {}; // Minus 8 bytes

            std::array<char, 4> waveTag {};

            std::array<char, 4> fmtTag {};
        } header {};

        WAV wav { path };
        BinaryInFile& file = wav.mFile;

        if (!file.read(header))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

        if (file.length() - 8 != static_cast<std::streamsize>(header.fileSize)) {
            Debug::err("File size does not match WAV header");
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        if (header.riffTag != std::array{'R', 'I', 'F', 'F'}) {
            Debug::err("Invalid 'RIFF' tag (Got {})", header.riffTag);
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        if (header.waveTag != std::array{'W', 'A', 'V', 'E'}) {
            Debug::err("Invalid 'WAVE' tag (Got {})", header.waveTag);
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        if (header.fmtTag != std::array{'f', 'm', 't', ' '}) {
            Debug::err("Invalid 'fmt ' tag (Got {})", header.fmtTag);
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        if (!file.read(wav.mFmt))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

        if (wav.mFmt.sampleRate != sampleRate) {
            Debug::err("Invalid WAV Sample Rate! (Got {}, Expected {})", wav.mFmt.sampleRate, sampleRate);
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }
        if (wav.mFmt.channels != 2) {
            Debug::err("Expected 2 WAV channels, not {}!", wav.mFmt.channels);
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        Debug::log<1>(
            "Loaded WAV File: {}\n"
            "Format Data Length: {}\n"
            "Block Size: {}\n"
            "Type of Format: {}\n"
            "Number of Channels: {}\n"
            "Sample Rate: {}\n"
            "Bytes Per Sec: {}\n"
            "Bytes Per Block: {}\n"
            "Bits Per Sample: {}",
            path,
            header.fileSize,
            wav.mFmt.blockSize,
            wav.mFmt.audioFormat,
            wav.mFmt.channels,
            wav.mFmt.sampleRate,
            wav.mFmt.bytesPerSec,
            wav.mFmt.bytesPerBlock,
            wav.mFmt.bitsPerSample
        );

        while (file) {
            struct ChunkHeader {
                std::array<char, 4> identifier {};
                std::uint32_t size {}; // (excluding 8 byte header)
            };

            ChunkHeader chunkHeader {};
            if (!file.read(chunkHeader))
                return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

            Debug::log<1>("Chunk size: {}", chunkHeader.size);

            if (chunkHeader.identifier == std::array{'d', 'a', 't', 'a'}) {
                wav.mDataOffset = file.tell();
                break;
            }

            // Skip chunk
            if (!file.seek(static_cast<long>(chunkHeader.size), std::ios::cur))
                return std::unexpected{ Failure{ ErrorCode::file_seek_fail } };
        }

        if (!wav.mDataOffset) {
            Debug::err("Failed to find WAV 'data' chunk!");
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        return std::make_shared<WAV>(std::move(wav));
    }
}
