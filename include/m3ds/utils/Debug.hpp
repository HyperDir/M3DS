#pragma once

#include <iostream>
#include <utility>
#include <filesystem>
#include <print>

namespace M3DS::Debug {
    inline unsigned short debugLevel = 0;

    inline void setLevel(unsigned short to) noexcept {
        debugLevel = to;
    }

    template <unsigned short level = 0>
    void log(std::formattable<char> auto&& str) noexcept;

    template <unsigned short level = 0, std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void log(std::format_string<Args...> str, Args&&... args) noexcept;

    void warn(std::formattable<char> auto&& str) noexcept;

    template <std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void warn(std::format_string<Args...> str, Args&&... args) noexcept;

    void err(std::formattable<char>auto&& str) noexcept;

    template <std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void err(std::format_string<Args...> str, Args&&... args) noexcept;

    [[noreturn]] void terminate() noexcept;

    [[noreturn]] void terminate(std::formattable<char> auto&& str) noexcept;

    template <std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    [[noreturn]] void terminate(std::format_string<Args...> str, Args&&... args) noexcept;
}


// Implementation
namespace M3DS::Debug {
    template <unsigned short level>
    void log(std::formattable<char> auto&& str) noexcept {
        if (debugLevel >= level)
#ifdef __3DS__
            std::format_to(std::ostreambuf_iterator{std::cout}, "{}\n", str);
#else
            std::format_to(std::ostreambuf_iterator{std::cout}, "[Info] {}\n", str);
#endif
    }

    template <unsigned short level, std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void log(const std::format_string<Args...> str, Args&&... args) noexcept {
        if (debugLevel >= level) {
#ifndef __3DS__
            std::cout << "[Info] ";
#endif
            std::format_to(std::ostreambuf_iterator{std::cout}, str, std::forward<Args>(args)...);
            std::cout << '\n';
        }
    }


    void warn(std::formattable<char> auto&& str) noexcept {
#ifdef __3DS__
        std::format_to(std::ostreambuf_iterator{std::cout}, "\x1b[33m{}\n\x1b[37m", str);
#else
        std::format_to(std::ostreambuf_iterator{std::cout}, "[Warning] {}\n\x1b[37m", str);
#endif
    }

    template <std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void warn(const std::format_string<Args...> str, Args&&... args) noexcept {
#ifdef __3DS__
        std::cerr << "\x1b[33m";
#else
        std::cerr << "[Warning] ";
#endif
        std::format_to(std::ostreambuf_iterator{std::cerr}, str, std::forward<Args>(args)...);
        std::cerr << "\n\x1b[37m";
    }


    void err(std::formattable<char> auto&& str) noexcept {
#ifdef __3DS__
        std::format_to(std::ostreambuf_iterator{std::cerr}, "\x1b[31m{}\n\x1b[37m", str);
#else
        std::format_to(std::ostreambuf_iterator{std::cerr}, "[ErrorCode] {}\n\x1b[37m", str);
#endif
    }

    template <std::formattable<char>... Args>
    requires (sizeof...(Args) > 0)
    void err(const std::format_string<Args...> str, Args&&... args) noexcept {
#ifdef __3DS__
        std::cerr << "\x1b[31m";
#else
        std::cerr << "[ErrorCode] ";
#endif
        std::format_to(std::ostreambuf_iterator{std::cerr}, str, std::forward<Args>(args)...);
        std::cerr << "\n\x1b[37m";
    }

    inline void terminate() noexcept {
        std::terminate();
    }

    void terminate(std::formattable<char> auto&& str) noexcept {
        err(str);
        std::terminate();
    }

    template <std::formattable<char>... Args>
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