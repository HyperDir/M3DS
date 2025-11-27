#pragma once

#include <m3ds/utils/Texture.hpp>
#include <m3ds/reference/Resource.hpp>

namespace M3DS {
    struct Tile {
        Rect2 tile = {0, 0, 16, 16};
        Rect2 collision = {0, 0, 16, 16};
    };

    class TileSet : public Resource {
        M_CLASS(TileSet, Resource)
    public:
        std::vector<Tile> tiles {};
        Texture texture {};
    };
}
