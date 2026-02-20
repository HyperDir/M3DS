#pragma once

#include <vector>
#include <functional>

#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/utils/Debug.hpp>

namespace M3DS {
    class Deferrer {
    public:
        [[nodiscard]] constexpr Deferrer() noexcept = default;
        constexpr ~Deferrer() noexcept;

        Deferrer(const Deferrer&) = delete;
        Deferrer& operator=(const Deferrer&) = delete;

        Deferrer(Deferrer&&) = delete;
        Deferrer& operator=(Deferrer&&) = delete;

        constexpr void defer(std::move_only_function<void()> func);
    private:
        std::vector<std::move_only_function<void()>> mDeferred {};
    };

    class Serialiser : public BinaryOutFileAccessor, public Deferrer {
        std::vector<std::move_only_function<void()>> mDeferred {};
    public:
        [[nodiscard]] explicit Serialiser(BinaryOutFileAccessor file) noexcept;
    };

    class Deserialiser : public BinaryInFileAccessor, public Deferrer {
        std::vector<std::move_only_function<void()>> mDeferred {};
    public:
        [[nodiscard]] explicit Deserialiser(BinaryInFileAccessor file) noexcept;
    };
}

/* Implementation */
namespace M3DS {
    constexpr Deferrer::~Deferrer() noexcept {
        for (auto& func : mDeferred)
            func();
    }

    constexpr void Deferrer::defer(std::move_only_function<void()> func) {
#ifdef __cpp_exceptions
        try {
#endif
            mDeferred.emplace_back(std::move(func));
#ifdef __cpp_exceptions
        } catch (...) {
            Debug::err("Exception thrown during Serialiser::defer");
        }
#endif
    }

    inline Serialiser::Serialiser(const BinaryOutFileAccessor file) noexcept
        : BinaryOutFileAccessor(file)
    {}

    inline Deserialiser::Deserialiser(const BinaryInFileAccessor file) noexcept
        : BinaryInFileAccessor(file)
    {}
}
