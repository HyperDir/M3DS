#pragma once

#include <filesystem>
#include <optional>

namespace M3DS {
    class BinaryFileAccessor;
    class BinaryInFileAccessor;
    class BinaryOutFileAccessor;

    template <typename T>
    concept TrivialIOType =
        std::is_trivially_copyable_v<T> &&
        std::is_standard_layout_v<T> &&
        !std::is_pointer_v<T> &&
        !std::is_reference_v<T> &&
        !std::is_const_v<T>;

    class InvalidBinaryFileException final : public std::exception {
    public:
        [[nodiscard]] const char* what() const noexcept override {
            return "Tried to get reference to invalid BinaryFileOpener file!";
        }
    };

    class BinaryFile {
    public:
        [[nodiscard]] BinaryFile() noexcept = default;
        [[nodiscard]] explicit BinaryFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]] constexpr bool isOpen() const noexcept;
        [[nodiscard]] explicit constexpr operator bool() const noexcept;

        [[nodiscard]] constexpr std::optional<BinaryFileAccessor> getAccessorIfValid() & noexcept;
        [[nodiscard]] constexpr BinaryFileAccessor getAccessor() & noexcept;

        [[nodiscard]] constexpr FILE* getNativeHandle() & noexcept;

        template <TrivialIOType T> [[nodiscard]] bool read(T& to) noexcept;
        template <TrivialIOType T> [[nodiscard]] bool read(std::span<T> to) noexcept;

        template <TrivialIOType T> [[nodiscard]] bool write(const T& to) noexcept;
        template <TrivialIOType T> [[nodiscard]] bool write(std::span<const T> to) noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;

        void close() noexcept;
    private:
        std::unique_ptr<std::FILE, decltype([](std::FILE* file) { if (file) std::fclose(file); })> mFile;
    };

    class BinaryInFile {
    public:
        [[nodiscard]] BinaryInFile() noexcept = default;
        [[nodiscard]] explicit BinaryInFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]] constexpr bool isOpen() const noexcept;
        [[nodiscard]] explicit constexpr operator bool() const noexcept;

        [[nodiscard]] constexpr std::optional<BinaryInFileAccessor> getAccessorIfValid() & noexcept;
        [[nodiscard]] constexpr BinaryInFileAccessor getAccessor() & noexcept;

        [[nodiscard]] constexpr FILE* getNativeHandle() & noexcept;

        template <TrivialIOType T> [[nodiscard]] bool read(T& to) noexcept;
        template <TrivialIOType T> [[nodiscard]] bool read(std::span<T> to) noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;

        void close() noexcept;
    private:
        std::unique_ptr<std::FILE, decltype([](std::FILE* file) { if (file) std::fclose(file); })> mFile;
    };

    class BinaryOutFile {
    public:
        [[nodiscard]] BinaryOutFile() noexcept = default;
        [[nodiscard]] explicit BinaryOutFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]] constexpr bool isOpen() const noexcept;
        [[nodiscard]] explicit constexpr operator bool() const noexcept;

        [[nodiscard]] constexpr std::optional<BinaryOutFileAccessor> getAccessorIfValid() & noexcept;
        [[nodiscard]] constexpr BinaryOutFileAccessor getAccessor() & noexcept;

        [[nodiscard]] constexpr FILE* getNativeHandle() & noexcept;

        template <TrivialIOType T> [[nodiscard]] bool write(const T& to) noexcept;
        template <TrivialIOType T> [[nodiscard]] bool write(std::span<const T> to) noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;

        void close() noexcept;
    private:
        std::unique_ptr<std::FILE, decltype([](std::FILE* file) { if (file) std::fclose(file); })> mFile;
    };

    // A view onto the file held by BinaryFile
    class BinaryFileAccessor {
        friend class BinaryFile;
    public:
        template <TrivialIOType T> [[nodiscard]] bool read(T& to) const noexcept;
        template <TrivialIOType T> [[nodiscard]] bool read(std::span<T> to) const noexcept;

        template <TrivialIOType T> [[nodiscard]] bool write(const T& to) const noexcept;
        template <TrivialIOType T> [[nodiscard]] bool write(std::span<const T> to) const noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) const noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;

        [[nodiscard]] constexpr FILE* getNativeHandle() const noexcept;
    private:
        [[nodiscard]] explicit constexpr BinaryFileAccessor(FILE* file) noexcept;
        FILE* mFile {};
    };

    class BinaryInFileAccessor {
        friend class BinaryInFile;
    public:
        [[nodiscard]] constexpr BinaryInFileAccessor(BinaryFileAccessor ref) noexcept;

        template <TrivialIOType T> [[nodiscard]] bool read(T& to) const noexcept;
        template <TrivialIOType T> [[nodiscard]] bool read(std::span<T> to) const noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) const noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;
    private:
        [[nodiscard]] explicit constexpr BinaryInFileAccessor(FILE* file) noexcept;
        FILE* mFile;
    };

    class BinaryOutFileAccessor {
        friend class BinaryOutFile;
    public:
        [[nodiscard]] constexpr BinaryOutFileAccessor(BinaryFileAccessor ref) noexcept;

        template <TrivialIOType T> [[nodiscard]] bool write(const T& to) const noexcept;
        template <TrivialIOType T> [[nodiscard]] bool write(std::span<const T> to) const noexcept;

        [[nodiscard]] bool seek(long offset, std::ios_base::seekdir dir = std::ios_base::beg) const noexcept;
        [[nodiscard]] long tell() const noexcept;
        [[nodiscard]] long length() const noexcept;
    private:
        [[nodiscard]] explicit constexpr BinaryOutFileAccessor(FILE* file) noexcept;
        FILE* mFile;
    };
}

/* Implementation */
namespace M3DS {
    inline BinaryFile::BinaryFile(const std::filesystem::path& path) noexcept : mFile(
#ifdef _WIN32
            _wfopen(path.c_str(), L"r+b")
#else
            std::fopen(path.c_str(), "r+b")
#endif
    ) {}

    constexpr bool BinaryFile::isOpen() const noexcept {
        return static_cast<bool>(mFile);
    }

    constexpr BinaryFile::operator bool() const noexcept {
        return isOpen();
    }

    constexpr std::optional<BinaryFileAccessor> BinaryFile::getAccessorIfValid() & noexcept {
        if (mFile)
            return BinaryFileAccessor{ mFile.get() };
        return {};
    }

    constexpr BinaryFileAccessor BinaryFile::getAccessor() & noexcept {
        if (!mFile) {
#if __cpp_exceptions
            throw InvalidBinaryFileException{};
#else
            std::terminate();
#endif
        }

        return BinaryFileAccessor{ mFile.get() };
    }

    constexpr FILE* BinaryFile::getNativeHandle() & noexcept {
        return mFile.get();
    }

    inline bool BinaryFile::seek(const long offset, const std::ios_base::seekdir dir) noexcept {
        if (mFile) return getAccessor().seek(offset, dir);
        return false;
    }

    inline long BinaryFile::tell() const noexcept {
        if (mFile) return std::ftell(mFile.get());
        return false;
    }

    inline long BinaryFile::length() const noexcept {
        if (!mFile) return false;

        const auto pos = std::ftell(mFile.get());
        std::fseek(mFile.get(), 0, std::ios::end);
        const auto size = std::ftell(mFile.get());
        std::fseek(mFile.get(), pos, std::ios::beg);
        return size;
    }

    inline void BinaryFile::close() noexcept {
        mFile.reset();
    }

    template <TrivialIOType T>
    bool BinaryFile::read(T& to) noexcept {
        if (mFile) return getAccessor().read(to);
        return false;
    }

    template <TrivialIOType T>
    bool BinaryFile::read(std::span<T> to) noexcept {
        if (mFile) return getAccessor().read(to);
        return false;
    }

    template <TrivialIOType T>
    bool BinaryFile::write(const T& to) noexcept {
        if (mFile) return getAccessor().write(to);
        return false;
    }

    template <TrivialIOType T>
    bool BinaryFile::write(std::span<const T> to) noexcept {
        if (mFile) return getAccessor().write(to);
        return false;
    }



    inline BinaryInFile::BinaryInFile(const std::filesystem::path& path) noexcept : mFile(
#ifdef _WIN32
            _wfopen(path.c_str(), L"rb")
#else
            std::fopen(path.c_str(), "rb")
#endif
    ) {}

    constexpr bool BinaryInFile::isOpen() const noexcept {
        return static_cast<bool>(mFile);
    }

    constexpr BinaryInFile::operator bool() const noexcept {
        return isOpen();
    }

    constexpr std::optional<BinaryInFileAccessor> BinaryInFile::getAccessorIfValid() & noexcept {
        if (mFile)
            return BinaryInFileAccessor{ mFile.get() };
        return {};
    }

    constexpr BinaryInFileAccessor BinaryInFile::getAccessor() & noexcept {
        if (!mFile) {
#if __cpp_exceptions
            throw InvalidBinaryFileException{};
#else
            std::terminate();
#endif
        }

        return BinaryInFileAccessor{ mFile.get() };
    }

    constexpr FILE* BinaryInFile::getNativeHandle() & noexcept {
        return mFile.get();
    }

    constexpr bool BinaryOutFile::isOpen() const noexcept {
        return static_cast<bool>(mFile);
    }

    constexpr BinaryOutFile::operator bool() const noexcept {
        return isOpen();
    }

    constexpr std::optional<BinaryOutFileAccessor> BinaryOutFile::getAccessorIfValid() & noexcept {
        if (mFile)
            return BinaryOutFileAccessor{ mFile.get() };
        return {};
    }

    constexpr BinaryOutFileAccessor BinaryOutFile::getAccessor() & noexcept {
        if (!mFile) {
#if __cpp_exceptions
            throw InvalidBinaryFileException{};
#else
            std::terminate();
#endif
        }

        return BinaryOutFileAccessor{ mFile.get() };
    }

    constexpr FILE* BinaryOutFile::getNativeHandle() & noexcept {
        return mFile.get();
    }

    inline bool BinaryOutFile::seek(const long offset, const std::ios_base::seekdir dir) noexcept {
        if (mFile) return getAccessor().seek(offset, dir);
        return false;
    }

    inline long BinaryOutFile::tell() const noexcept {
        if (mFile) return std::ftell(mFile.get());
        return 0;
    }

    inline long BinaryOutFile::length() const noexcept {
        if (!mFile) return 0;
        const auto pos = std::ftell(mFile.get());
        std::fseek(mFile.get(), 0, std::ios::end);
        const auto size = std::ftell(mFile.get());
        std::fseek(mFile.get(), pos, std::ios::beg);
        return size;
    }

    inline void BinaryOutFile::close() noexcept {
        mFile.reset();
    }

    template <TrivialIOType T>
    bool BinaryOutFile::write(const T& to) noexcept {
        if (mFile) return getAccessor().write(to);
        return false;
    }

    template <TrivialIOType T>
    bool BinaryOutFile::write(std::span<const T> to) noexcept {
        if (mFile) return getAccessor().write(to);
        return false;
    }

    inline bool BinaryInFile::seek(const long offset, const std::ios_base::seekdir dir) noexcept {
        if (mFile) return getAccessor().seek(offset, dir);
        return false;
    }

    inline long BinaryInFile::tell() const noexcept {
        if (mFile) return std::ftell(mFile.get());
        return 0;
    }

    inline long BinaryInFile::length() const noexcept {
        if (!mFile) return 0;

        const auto pos = std::ftell(mFile.get());
        std::fseek(mFile.get(), 0, std::ios::end);
        const auto size = std::ftell(mFile.get());
        std::fseek(mFile.get(), pos, std::ios::beg);
        return size;
    }

    inline void BinaryInFile::close() noexcept {
        mFile.reset();
    }

    inline BinaryOutFile::BinaryOutFile(const std::filesystem::path& path) noexcept
        : mFile(
#ifdef _WIN32
        _wfopen(path.c_str(), L"wb")
#else
        std::fopen(path.c_str(), "wb")
#endif
    ) {}

    template <TrivialIOType T>
    bool BinaryInFile::read(T& to) noexcept {
        if (mFile) return getAccessor().read(to);
        return false;
    }

    template <TrivialIOType T>
    bool BinaryInFile::read(std::span<T> to) noexcept {
        if (mFile) return getAccessor().read(to);
        return false;
    }


    constexpr FILE* BinaryFileAccessor::getNativeHandle() const noexcept {
        return mFile;
    }

    constexpr BinaryFileAccessor::BinaryFileAccessor(FILE* file) noexcept : mFile(file) {}

    template <TrivialIOType T>
    bool BinaryFileAccessor::read(T& to) const noexcept {
        return std::fread(&to, sizeof(T), 1, mFile) == 1;
    }

    template <TrivialIOType T>
    bool BinaryFileAccessor::read(std::span<T> to) const noexcept {
        return std::fread(to.data(), sizeof(T), to.size(), mFile) == to.size();
    }

    template <TrivialIOType T>
    bool BinaryFileAccessor::write(const T& to) const noexcept {
        return std::fwrite(&to, sizeof(T), 1, mFile) == 1;
    }

    template <TrivialIOType T>
    bool BinaryFileAccessor::write(std::span<const T> to) const noexcept {
        return std::fwrite(to.data(), sizeof(T), to.size(), mFile) == to.size();
    }

    template <TrivialIOType T>
    bool BinaryInFileAccessor::read(T& to) const noexcept {
        return std::fread(&to, sizeof(T), 1, mFile) == 1;
    }

    template <TrivialIOType T>
    bool BinaryInFileAccessor::read(std::span<T> to) const noexcept {
        return std::fread(to.data(), sizeof(T), to.size(), mFile) == to.size();
    }

    template <TrivialIOType T>
    bool BinaryOutFileAccessor::write(const T& to) const noexcept {
        return std::fwrite(&to, sizeof(T), 1, mFile) == 1;
    }

    template <TrivialIOType T>
    bool BinaryOutFileAccessor::write(std::span<const T> to) const noexcept {
        return std::fwrite(to.data(), sizeof(T), to.size(), mFile) == to.size();
    }

    inline bool BinaryFileAccessor::seek(const long offset, const std::ios_base::seekdir dir) const noexcept {
        return std::fseek(mFile, offset, dir) == 0;
    }

    inline long BinaryFileAccessor::tell() const noexcept {
        return std::ftell(mFile);
    }

    inline long BinaryFileAccessor::length() const noexcept {
        const auto pos = std::ftell(mFile);
        std::fseek(mFile, 0, std::ios::end);
        const auto size = std::ftell(mFile);
        std::fseek(mFile, pos, std::ios::beg);
        return size;
    }

    inline bool BinaryInFileAccessor::seek(const long offset, const std::ios_base::seekdir dir) const noexcept {
        return std::fseek(mFile, offset, dir) == 0;
    }

    inline long BinaryInFileAccessor::tell() const noexcept {
        return std::ftell(mFile);
    }

    inline long BinaryInFileAccessor::length() const noexcept {
        const auto pos = std::ftell(mFile);
        std::fseek(mFile, 0, std::ios::end);
        const auto size = std::ftell(mFile);
        std::fseek(mFile, pos, std::ios::beg);
        return size;
    }

    inline bool BinaryOutFileAccessor::seek(const long offset, const std::ios_base::seekdir dir) const noexcept {
        return std::fseek(mFile, offset, dir) == 0;
    }

    inline long BinaryOutFileAccessor::tell() const noexcept {
        return std::ftell(mFile);
    }

    inline long BinaryOutFileAccessor::length() const noexcept {
        const auto pos = std::ftell(mFile);
        std::fseek(mFile, 0, std::ios::end);
        const auto size = std::ftell(mFile);
        std::fseek(mFile, pos, std::ios::beg);
        return size;
    }


    constexpr BinaryInFileAccessor::BinaryInFileAccessor(const BinaryFileAccessor ref) noexcept : mFile(ref.getNativeHandle()) {}
    constexpr BinaryInFileAccessor::BinaryInFileAccessor(FILE* file) noexcept : mFile(file) {}

    constexpr BinaryOutFileAccessor::BinaryOutFileAccessor(const BinaryFileAccessor ref) noexcept : mFile(ref.getNativeHandle()) {}
    constexpr BinaryOutFileAccessor::BinaryOutFileAccessor(FILE* file) noexcept : mFile(file) {}
}
