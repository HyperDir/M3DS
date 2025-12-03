#pragma once

#ifdef __3DS__
#include <cstdint>
#include <span>
#include <utility>

#include <citro3d.h>

namespace M3DS {
    class DVLB {
        DVLB_s* dvlb {};
    public:
        constexpr DVLB() noexcept = default;

        explicit DVLB(std::span<unsigned char> shader) noexcept;
        explicit DVLB(std::span<std::uint32_t> shader) noexcept;

        constexpr DVLB(const DVLB&) noexcept = delete;
        constexpr DVLB& operator=(const DVLB&) noexcept = delete;

        constexpr DVLB(DVLB&& other) noexcept;

        constexpr DVLB& operator=(DVLB&& other) noexcept;

        ~DVLB() noexcept;

        [[nodiscard]] constexpr DVLB_s* get() noexcept;
        [[nodiscard]] constexpr const DVLB_s* get() const noexcept;

        DVLB_s* operator->() noexcept;
        const DVLB_s* operator->() const noexcept;

        DVLB_s& operator*() noexcept;
        const DVLB_s& operator*() const noexcept;
    };
}

/* Implementations */
namespace M3DS {
    inline DVLB::DVLB(const std::span<unsigned char> shader) noexcept
        : dvlb(DVLB_ParseFile(reinterpret_cast<std::uint32_t*>(shader.data()), shader.size_bytes()))
    {}

    inline DVLB::DVLB(const std::span<std::uint32_t> shader) noexcept
        : dvlb(DVLB_ParseFile(shader.data(), shader.size_bytes()))
    {}

    constexpr DVLB::DVLB(DVLB&& other) noexcept
        : dvlb(std::exchange(other.dvlb, nullptr))
    {}

    constexpr DVLB& DVLB::operator=(DVLB&& other) noexcept {
        if (this != &other)
            dvlb = std::exchange(other.dvlb, nullptr);
        return *this;
    }

    inline DVLB::~DVLB() noexcept {
        DVLB_Free(dvlb);
    }

    constexpr DVLB_s* DVLB::get() noexcept {
        return dvlb;
    }

    constexpr const DVLB_s* DVLB::get() const noexcept {
        return dvlb;
    }

    inline DVLB_s* DVLB::operator->() noexcept {
        return dvlb;
    }

    inline const DVLB_s* DVLB::operator->() const noexcept {
        return dvlb;
    }

    inline DVLB_s& DVLB::operator*() noexcept {
        return *dvlb;
    }

    inline const DVLB_s& DVLB::operator*() const noexcept {
        return *dvlb;
    }
}
#endif
