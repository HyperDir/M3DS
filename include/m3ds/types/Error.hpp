#pragma once

#include <cstdint>
#include <cassert>
#include <string>

#include <source_location>
#include <format>
#include <utility>

namespace M3DS {
    using StringError = std::string;

    enum class ErrorCode : std::uint16_t {
        none = 0,
        file_open_fail,
        file_write_fail,
        file_read_fail,
        file_seek_fail,
        file_invalid_data,
        directory_not_found,
        failed_to_get_path_to_node,
        allocation_failed,
        integer_too_large,
        invalid_class_name,
        non_default_constructible_class,
        root_serialisation_disabled,
        out_of_bounds,
        object_cast_failed,
        not_implemented,
        no_resource_path,
        type_mismatch,
        object_conversion_failure
    };
}



template <>
struct std::formatter<M3DS::ErrorCode> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const M3DS::ErrorCode& code, FormatContext& ctx) const {
        constexpr std::array errorCodes = std::to_array<std::string_view>({
            "None",
            "File open fail",
            "File write fail",
            "File read fail",
            "File seek fail",
            "File invalid data",
            "Directory not found",
            "Failed to get path to node",
            "Allocation failed",
            "Integer too large",
            "Invalid class name",
            "Non default constructible class",
            "Root serialisation is disabled",
            "Out of bounds",
            "Object cast failed",
            "Not implemented",
            "No resource path",
            "Type mismatch",
            "Object conversion failed"
        });

        assert(std::to_underlying(code) < errorCodes.size());

        return std::formatter<std::string_view>::format(errorCodes[std::to_underlying(code)], ctx);
    }
};

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