#pragma once

#include <source_location>
#include <format>

#include <m3ds/types/ErrorCode.hpp>

namespace M3DS {
    // Perhaps strip location from release builds?
    struct Failure {
        ErrorCode error {};
        std::source_location location = error == ErrorCode::none ? std::source_location{} : std::source_location::current();

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return error != ErrorCode::none;
        }
    };

    static constexpr Failure Success { ErrorCode::none };
}

template <>
struct std::formatter<M3DS::Failure> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const M3DS::Failure& failure, FormatContext& ctx) const {
        if (failure.error == M3DS::ErrorCode::none) {
            return std::format_to(ctx.out(), "No errors occurred");
        }
        return std::format_to(
            ctx.out(),
            "{}:{}:{} in function {} failed with error '{}'!",
            failure.location.file_name(),
            failure.location.line(),
            failure.location.column(),
            failure.location.function_name(),
            failure.error
        );
    }
};