#pragma once

#include <memory>

namespace M3DS {
    template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
    std::unique_ptr<T> make_unique_nothrow(Args&&... args) {
        return std::unique_ptr<T>(new (std::nothrow) T{ std::forward<Args>(args)... });
    }

    template <typename T>
    requires (std::is_array_v<T>)
    std::unique_ptr<T> make_unique_nothrow(std::size_t size) {
        return std::unique_ptr<T>(new (std::nothrow) std::remove_extent_t<T>[size]{});
    }


    template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
    std::unique_ptr<T> make_unique_for_overwrite_nothrow() {
        return std::unique_ptr<T>(new (std::nothrow) T);
    }

    template <typename T>
    requires (std::is_array_v<T>)
    std::unique_ptr<T> make_unique_for_overwrite_nothrow(std::size_t size) {
        return std::unique_ptr<T>(new (std::nothrow) std::remove_extent_t<T>[size]);
    }
}
