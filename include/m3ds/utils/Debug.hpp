#pragma once

#include <iostream>
#include <utility>
#include <filesystem>
#include <print>

#include <m3ds/types/Float16.hpp>

namespace M3DS::Debug {
    inline unsigned short debugLevel = 0;

    inline void setLevel(unsigned short to) noexcept {
        debugLevel = to;
    }

    template <unsigned short level = 0>
    void log(auto&& str) noexcept;

    template <unsigned short level = 0, typename... Args>
    requires (sizeof...(Args) > 0)
    void log(std::format_string<Args...> str, Args&&... args) noexcept;

    void warn(auto&& str) noexcept;

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    void warn(std::format_string<Args...> str, Args&&... args) noexcept;

    void err(auto&& str) noexcept;

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    void err(std::format_string<Args...> str, Args&&... args) noexcept;

    [[noreturn]] void terminate() noexcept;

    [[noreturn]] void terminate(auto&& str) noexcept;

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    [[noreturn]] void terminate(std::format_string<Args...> str, Args&&... args) noexcept;
}


// Implementation
namespace M3DS::Debug {
    template <unsigned short level>
    void log([[maybe_unused]] auto&& str) noexcept {
        if (debugLevel >= level)
#ifdef __3DS__
            std::format_to(std::ostreambuf_iterator{std::cout}, "{}\n", str);
#elifdef M3DS_SFML
            std::format_to(std::ostreambuf_iterator{std::cout}, "[Info] {}\n", str);
#endif
    }

    template <unsigned short level, typename... Args>
    requires (sizeof...(Args) > 0)
    void log(const std::format_string<Args...> str, Args&&... args) noexcept {
        if (debugLevel >= level) {
#ifdef M3DS_SFML
            std::cout << "[Info] ";
#endif
            std::format_to(std::ostreambuf_iterator{std::cout}, str, std::forward<Args>(args)...);
            std::cout << '\n';
        }
    }


    void warn([[maybe_unused]] auto&& str) noexcept {
#ifdef __3DS__
        std::format_to(std::ostreambuf_iterator{std::cout}, "\x1b[33m{}\n\x1b[37m", str);
#elifdef M3DS_SFML
        std::format_to(std::ostreambuf_iterator{std::cout}, "[Warning] {}\n\x1b[37m", str);
#endif
    }

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    void warn(const std::format_string<Args...> str, Args&&... args) noexcept {
#ifdef __3DS__
        std::cerr << "\x1b[33m";
#elifdef M3DS_SFML
        std::cerr << "[Warning] ";
#endif
        std::format_to(std::ostreambuf_iterator{std::cerr}, str, std::forward<Args>(args)...);
        std::cerr << "\n\x1b[37m";
    }


    void err([[maybe_unused]] auto&& str) noexcept {
#ifdef __3DS__
        std::format_to(std::ostreambuf_iterator{std::cerr}, "\x1b[31m{}\n\x1b[37m", str);
#elifdef M3DS_SFML
        std::format_to(std::ostreambuf_iterator{std::cerr}, "[Error] {}\n\x1b[37m", str);
#endif
    }

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    void err(const std::format_string<Args...> str, Args&&... args) noexcept {
#ifdef __3DS__
        std::cerr << "\x1b[31m";
#elifdef M3DS_SFML
        std::cerr << "[Error] ";
#endif
        std::format_to(std::ostreambuf_iterator{std::cerr}, str, std::forward<Args>(args)...);
        std::cerr << "\n\x1b[37m";
    }

    inline void terminate() noexcept {
        std::terminate();
    }

    void terminate(auto&& str) noexcept {
        err(str);
        std::terminate();
    }

    template <typename... Args>
    requires (sizeof...(Args) > 0)
    void terminate(const std::format_string<Args...> str, Args&&... args) noexcept {
        terminate(std::format(str, std::forward<Args>(args)...));
    }
}


#ifndef __cpp_lib_format_path
template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    template <class FormatContext>
    auto format(const filesystem::path& path, FormatContext& ctx) const {
        if constexpr (std::same_as<filesystem::path::string_type, std::string>) {
            return std::formatter<std::string>::format(path.native(), ctx);
        } else {
            return std::formatter<std::string>::format(path.string(), ctx);
        }
    }
};
#endif