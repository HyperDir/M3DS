#pragma once

namespace M3DS {
    template <typename... Args>
    struct Visitor : Args... {
        using Args::operator() ...;
    };
}
