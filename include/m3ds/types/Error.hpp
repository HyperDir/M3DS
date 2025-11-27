#pragma once

#include <string>

namespace M3DS {
    using StringError = std::string;

    enum class Error : std::uint16_t {
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
