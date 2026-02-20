#include <m3ds/nodes/CanvasLayer.hpp>

namespace M3DS {
    CanvasLayer::CanvasLayer() {
        mCanvasLayer = this;
    }

    void CanvasLayer::draw(RenderTarget2D& target) {
        if (const std::optional<Vector2> cameraPos = target.getCameraPos()) {
            target.clearCamera();
            Node::draw(target);
            target.setCameraPos(*cameraPos);
        } else {
            Node::draw(target);
        }
    }

    Failure CanvasLayer::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure CanvasLayer::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }



    REGISTER_NO_METHODS(CanvasLayer);
    REGISTER_NO_MEMBERS(CanvasLayer);
}
