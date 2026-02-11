#pragma once

#include <cstdint>
#include <string_view>
#include <format>
#include <array>
#include <cassert>
#include <utility>

namespace M3DS {
    enum class ErrorCode : std::uint16_t {
        none = 0,
        file_open_fail,
        file_write_fail,
        file_read_fail,
        file_seek_fail,
        invalid_data,
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
            "Invalid data",
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