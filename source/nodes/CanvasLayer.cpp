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

    Failure CanvasLayer::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Failure CanvasLayer::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_NO_METHODS(CanvasLayer);
    REGISTER_NO_MEMBERS(CanvasLayer);
}
