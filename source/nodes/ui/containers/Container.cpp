#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    // void Container::draw(RenderTarget2D& target) noexcept {
    //     UINode::draw(target);
    //
    //     const Vector2& pos = getGlobalTransform().position;
    //
    //     constexpr auto from = static_cast<uint32_t>(Colours::red);
    //     constexpr auto to = static_cast<uint32_t>(Colours::orange);
    //
    //     C2D_DrawLine(
    //         pos.x,
    //         pos.y,
    //         from,
    //         pos.x + getSize().x,
    //         pos.y,
    //         to,
    //         1,
    //         1
    //     );
    //     C2D_DrawLine(
    //         pos.x,
    //         pos.y,
    //         from,
    //         pos.x,
    //         pos.y + getSize().y,
    //         to,
    //         1,
    //         1
    //     );
    //     C2D_DrawLine(
    //         pos.x,
    //         pos.y + getSize().y,
    //         to,
    //         pos.x + getSize().x,
    //         pos.y + getSize().y,
    //         from,
    //         1,
    //         1
    //     );
    //     C2D_DrawLine(
    //         pos.x + getSize().x,
    //         pos.y,
    //         to,
    //         pos.x + getSize().x,
    //         pos.y + getSize().y,
    //         from,
    //         1,
    //         1
    //     );
    // }

    Vector2 Container::getChildrenMaxSize() noexcept {
        Vector2 maxSize {};
        for (const auto* child : getChildrenOfType<UINode>()) {
            maxSize = max(maxSize, child->getMinSize());
        }
        return maxSize;
    }

    Error Container::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error Container::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_NO_METHODS(Container);
    REGISTER_NO_MEMBERS(Container);
}
