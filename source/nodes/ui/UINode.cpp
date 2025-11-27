#include <m3ds/nodes/ui/UINode.hpp>

#include <stack>

namespace M3DS {
    static std::array<UINode*, 8> focused {};

    bool UINode::needsResize() const noexcept {
        return mNeedsResize;
    }

    void UINode::queueResize() noexcept {
        for (
            auto curr = this;
            curr && !curr->mNeedsResize;
            curr = object_cast<UINode*>(curr->getParent())
        ) {
            curr->mNeedsResize = true;
        }
    }

    void UINode::takeFocus(const std::uint8_t idx) noexcept {
        if (idx > 8)
            return;
        if (focused[idx])
            focused[idx]->clearFocus(idx);
        focused[idx] = this;
        mFocus.set(idx);
    }

    void UINode::clearFocus(const std::uint8_t idx) noexcept {
        if (idx > 8)
            return;
        focused[idx] = nullptr;
        mFocus.clear(idx);
    }

    void UINode::update(const Seconds<float> delta) {
        CanvasItem::update(delta);

        resizeTree();
    }

    void UINode::draw(RenderTarget2D& target) {
        if (mFocus) {
            std::uint16_t r {};
            std::uint16_t g {};
            std::uint16_t b {};

            std::uint8_t count {};

            for (std::uint8_t i{}; i < static_cast<std::uint8_t>(focusColours.size()); ++i) {
                if (mFocus.get(i)) {
                    ++count;
                    r += focusColours[i].r;
                    g += focusColours[i].g;
                    b += focusColours[i].b;
                }
            }

            target.drawRectSolid(
                getGlobalTransform(),
                mSize + Vector2{2},
                {
                    static_cast<std::uint8_t>(r / count),
                    static_cast<std::uint8_t>(g / count),
                    static_cast<std::uint8_t>(b / count)
                }
            );
        }

        if (getClipContent()) {
            const std::optional<Rect2> prevScissor = target.getScissor();
            // const Vector2& pos = getGlobalTransform().position;
            const Vector2& size = getSize();

            // const std::optional<Vector2> cameraPos = target.getCameraPos();
            // const Vector2 cameraOffset = cameraPos ? -*cameraPos : Vector2{};

            const Vector2 screenPos = getScreenPosition();

            target.setScissor({
                screenPos.x,
                screenPos.y,
                size.x,
                size.y
            });

            CanvasItem::draw(target);

            if (prevScissor) {
                target.setScissor(*prevScissor);
            } else {
                target.clearScissor();
            }
        } else {
            CanvasItem::draw(target);
        }
    }

    void UINode::shrinkToFit(const Vector2& size) noexcept {
        mSize = size;
        queueResize();
    }

    void UINode::afterTreeEnter() {
        CanvasItem::afterTreeEnter();

        queueResize();
    }

    void UINode::shrink() noexcept {
        mSize = getMinSize();
        queueResize();
    }

    void UINode::resize() noexcept {
        mSize = max(mSize, getMinSize());

        Debug::log<1>("Resized {}", getName());
    }

    bool UINode::isOccluded(const Vector2& point) const noexcept {
        if (getClipContent()) {
            const Vector2& pos = getGlobalTransform().position;
            const Vector2& size = getSize();

            const Vector2 relativeCursor = point - pos;

            if (
                relativeCursor.x < 0 || relativeCursor.y < 0 ||
                relativeCursor.x > size.x || relativeCursor.y > size.y
            ) {
                return true;
            }
        }

        if (auto* parent = object_cast<const UINode*>(getParent())) {
            return parent->isOccluded(point);
        }
        return false;
    }

    void UINode::updateMinSize() noexcept {
        mInternalMinSize = {};
    }

    const Vector2& UINode::getSize() const noexcept {
        return mSize;
    }

    Vector2 UINode::getMinSize() const noexcept {
        return max(mInternalMinSize, mUserMinSize);
    }

    void UINode::setFillSpace(const bool to) noexcept {
        if (mFillSpace != to) {
            mFillSpace = to;
            queueResize();
        }
    }

    bool UINode::getFillSpace() const noexcept {
        return mFillSpace;
    }

    void UINode::setClipContent(const bool to) noexcept {
        mClipContent = to;
    }

    bool UINode::getClipContent() const noexcept {
        return mClipContent;
    }

    Error UINode::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = CanvasItem::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(mSize) ||
            !file.write(mUserMinSize) ||
            !file.write(mFillSpace) ||
            !file.write(mClipContent)
        )
            return Error::file_write_fail;

        for (const NodePath* neighbour : { &neighbours.up, &neighbours.down, &neighbours.left, &neighbours.right }) {
            if (const Error error = neighbour->serialise(file); error != Error::none)
                return error;
        }

        return Error::none;
    }

    Error UINode::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = CanvasItem::deserialise(file); error != Error::none)
            return error;

        if (
            !file.read(mSize) ||
            !file.read(mUserMinSize) ||
            !file.read(mFillSpace) ||
            !file.read(mClipContent)
        )
            return Error::file_write_fail;

        for (NodePath* neighbour : { &neighbours.up, &neighbours.down, &neighbours.left, &neighbours.right }) {
            if (const Error error = neighbour->deserialise(file); error != Error::none)
                return error;
        }

        mNeedsResize = true;

        return Error::none;
    }

    void UINode::setSize(const Vector2& to) noexcept {
        mSize = max(getMinSize(), to);

        queueResize();
    }

    void UINode::setMinSize(const Vector2& to) noexcept {
        mUserMinSize = to;

        queueResize();
    }

    void UINode::resizeTree() noexcept {
        if (!mNeedsResize)
            return;

        const auto updateMinSizes = [](this auto&& self, UINode* node) -> void {
            for (auto* child : node->getChildrenOfType<UINode>())
                if (child->needsResize())
                    self(child);
            node->updateMinSize();
        };

        const auto resizeFunc = [](this auto&& self, UINode* node) -> void {
            node->resize();
            node->mNeedsResize = false;
            for (auto* child : node->getChildrenOfType<UINode>())
                if (child->needsResize())
                    self(child);
        };

        UINode* curr = this;
        while (true) {
            UINode* tmp = object_cast<UINode*>(curr->getParent());
            if (!tmp || !tmp->needsResize())
                break;
            curr = tmp;
        }

        updateMinSizes(curr);
        resizeFunc(curr);
    }

    REGISTER_METHODS(
        UINode,
        CONST_METHOD(needsResize),
        MUTABLE_METHOD(queueResize),
        MUTABLE_METHOD(takeFocus),
        MUTABLE_METHOD(clearFocus),
        MUTABLE_METHOD(setSize),
        CONST_METHOD(getSize),
        MUTABLE_METHOD(setMinSize),
        CONST_METHOD(getMinSize),
        MUTABLE_METHOD(setFillSpace),
        CONST_METHOD(getFillSpace),
        MUTABLE_METHOD(setClipContent),
        CONST_METHOD(getClipContent)
    );

    REGISTER_MEMBERS(
        UINode,

        bindMember("needsResize", &UINode::needsResize),
        PRIVATE_MEMBER(size, getSize, setSize),
        PRIVATE_MEMBER(minSize, getMinSize, setMinSize),
        PRIVATE_MEMBER(fillSpace, getFillSpace, setFillSpace),
        PRIVATE_MEMBER(clipContent, getClipContent, setClipContent)
    );
}
