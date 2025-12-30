#include <m3ds/nodes/Node.hpp>

#include <queue>
#include <stack>

#include <m3ds/utils/Frame.hpp>

#include <m3ds/nodes/Root.hpp>

#include "m3ds/utils/binding/Registry.hpp"

#include <m3ds/nodes/Viewport.hpp>
#include <m3ds/nodes/CanvasLayer.hpp>

namespace M3DS {

    REGISTER_METHODS(
        Node,

        BOTH_METHOD(getParent),
        BOTH_METHOD(getRoot),
        CONST_METHOD(isInTree),
        BOTH_METHOD(getViewport),
        CONST_METHOD(getCanvasLayer),
        MUTABLE_METHOD(setName),
        CONST_METHOD(isHelper),
        BOTH_METHOD(getScript),
        BOTH_METHOD(getNode),
        BOTH_METHOD(getChild),
        CONST_METHOD(getPath),
        CONST_METHOD(getPathTo),
        CONST_METHOD(printTree),
        MUTABLE_METHOD(queueFree)
    );

    REGISTER_MEMBERS(
        Node,

        MEMBER(visible)
    );

    std::span<const std::unique_ptr<Node>> Node::getChildren() noexcept {
        return mChildren;
    }

    Node* Node::getParent() noexcept {
        return mParent;
    }

    const Node* Node::getParent() const noexcept {
        return mParent;
    }

    Root* Node::getRoot() noexcept {
        return mRoot;
    }

    const Root* Node::getRoot() const noexcept {
        return mRoot;
    }

    bool Node::isInTree() const noexcept {
        return getRoot() != nullptr;
    }

    void Node::setName(const std::string_view name) {
        mName = name;
    }

    std::string_view Node::getName() const noexcept {
        if (mName.empty())
            return getClass();
        return mName;
    }

    Node* Node::emplaceChild(std::unique_ptr<Node> node) {
        Node* child = mChildren.emplace_back(std::move(node)).get();

        child->mParent = this;
        if (isInTree())
            child->notification(Notification::tree_entered);

        return child;
    }

    void Node::update([[maybe_unused]] Seconds<float> delta) {}

    void Node::draw(RenderTarget2D& target) {
        if (mScript)
            mScript->draw(target);

        for (const std::unique_ptr<Node>& child : mChildren)
            child->draw(target);
    }

    void Node::draw(RenderTarget3D& target) {
        if (mScript)
            mScript->draw(target);

        for (const std::unique_ptr<Node>& child : mChildren)
            child->draw(target);
    }

    void Node::input([[maybe_unused]] Input::InputFrame& inputFrame) {}

    void Node::notification(const Notification notification) {
        if (notification == Notification::tree_entered) {
            if (const Node* parent = getParent()) {
                mCanvasLayer = parent->mCanvasLayer;
                if (mViewport != this)
                    mViewport = parent->mViewport;

                mRoot = parent->mRoot;
                if (mRoot)
                    mRoot->enableUpdate(this);
            }

            afterTreeEnter();
        } else if (notification == Notification::tree_exited) {
            beforeTreeExit();

            if (mViewport != this)
                mViewport = {};

            if (mCanvasLayer != this)
                mCanvasLayer = {};

            if (mRoot) {
                mRoot->disableUpdate(this);
                mRoot = {};
            }
        }
    }

    void Node::afterTreeEnter() {}
    void Node::beforeTreeExit() {}

    std::unique_ptr<Node> Node::removeChild(const Node* child) noexcept {
        std::unique_ptr<Node> ret {};

        std::erase_if(mChildren, [&](std::unique_ptr<Node>& node) {
            if (node.get() == child) {
                ret = std::move(node);
                return true;
            }
            return false;
        });

        ret->propagateNotification(Notification::tree_exited);

        return ret;
    }

    std::unique_ptr<Node> Node::removeChild(const std::size_t idx) noexcept {
        if (idx < mChildren.size())
            return removeChild(mChildren[idx].get());
        return {};
    }

    bool Node::isHelper() const noexcept {
        return mHelper;
    }

    BaseScript* Node::getScript() noexcept {
        return mScript.get();
    }

    const BaseScript* Node::getScript() const noexcept {
        return mScript.get();
    }

    Node* Node::getNode(const NodePath& path) noexcept {
        auto curr = this;
        for (const auto& node : path) {
            if (node == ".")
                continue;
            if (node == "..") {
                curr = curr->mParent;
                continue;
            }
            const auto it = std::ranges::find_if(
                curr->mChildren,
                [&node](const std::unique_ptr<Node>& child) {
                    return child->getName() == node;
                }
            );
            if (it == curr->mChildren.end())
                return {};
            curr = it->get();
        }
        return curr;
    }

    const Node* Node::getNode(const NodePath& path) const noexcept {
        auto curr = this;
        for (const auto& node : path) {
            if (node == ".")
                continue;
            if (node == "..") {
                curr = curr->mParent;
                continue;
            }
            const auto it = std::ranges::find_if(
                curr->mChildren,
                [&node](const std::unique_ptr<Node>& child) {
                    return child->mName == node;
                }
            );
            if (it == curr->mChildren.end())
                return {};
            curr = it->get();
        }
        return curr;
    }

    // Node* Node::getNode(std::string path) noexcept {
    //     return getNode(NodePath{ std::move(path) });
    // }
    //
    // const Node* Node::getNode(std::string path) const noexcept {
    //     return getNodeConst(NodePath{ std::move(path) });
    // }

    Node* Node::getChild(const std::size_t idx) noexcept {
        return mChildren[idx].get();
    }

    const Node* Node::getChild(const std::size_t idx) const noexcept {
        return mChildren[idx].get();
    }

    std::size_t Node::getChildCount() const noexcept {
        return mChildren.size();
    }

    NodePath Node::getPath() const {
        std::stack<const Node*> stack {};
        std::size_t characters {};
        for (auto curr = this; curr; curr = curr->mParent) {
            stack.emplace(curr);
            characters += curr->mName.size() + 1;
        }

        NodePath path { std::string{stack.top()->getName()} };
        path.reserve(characters);
        stack.pop();
        while (!stack.empty()) {
            path /= stack.top()->getName();
            stack.pop();
        }
        return path;
    }

    std::size_t getDepth(const Node* node) noexcept {
        std::size_t depth {};
        while (node) {
            ++depth;
            node = node->getParent();
        }
        return depth;
    }

    NodePath Node::getPathTo(const Node* other) const {
        if (!other || this == other)
            return {};

        const std::size_t selfDepth = getDepth(this);
        const std::size_t otherDepth = getDepth(other);
        const std::size_t minDepth = std::min(selfDepth, otherDepth);

        const auto [commonAncestor, commonAncestorDepth] = [&] {
            const Node* selfAncestor = this;
            const Node* otherAncestor = other;

            if (selfDepth != minDepth) {
                const std::size_t delta = selfDepth - minDepth;
                for (std::size_t i {}; i < delta; ++i)
                    selfAncestor = selfAncestor->mParent;
            } else if (otherDepth != minDepth) {
                const std::size_t delta = otherDepth - minDepth;
                for (std::size_t i {}; i < delta; ++i)
                    otherAncestor = otherAncestor->mParent;
            }

            std::size_t ancestorDepth = minDepth;
            while (selfAncestor != otherAncestor && selfAncestor) {
                selfAncestor = selfAncestor->mParent;
                otherAncestor = otherAncestor->mParent;
                --ancestorDepth;
            }
            return std::make_pair(selfAncestor, ancestorDepth);
        }();

        if (!commonAncestor)
            return {};

        const std::size_t upDistance = selfDepth - commonAncestorDepth;

        std::string path {};
        path.reserve(std::bit_ceil(upDistance * 3));

        for (std::size_t i {}; i < upDistance; ++i)
            path += "../";

        [&](this const auto& self, const Node* curr) -> void {
            if (curr != commonAncestor) {
                self(curr->mParent);
                path += curr->mName;
                path += '/';
            }
        }(other);

        path.pop_back();

        return NodePath{ std::move(path) };
    }

    void Node::printTree() const noexcept {
        std::stack<std::pair<const Node*, int>> stack {};
        stack.emplace(this, 0);

        while (!stack.empty()) {
            const auto [curr, depth] = stack.top();
            stack.pop();

            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << curr->getName() << std::endl;
            for (const auto& child : curr->mChildren | std::views::reverse)
                stack.emplace(child.get(), depth + 1);
        }
    }

    void Node::queueFree() {
        if (Root* root = getRoot())
            root->addToFreeQueue(this);
        else
            free();
    }

    void Node::free() {
        if (Node* parent = getParent())
            erase_if(parent->mChildren, [this](const std::unique_ptr<Node>& n) { return n.get() == this; });
    }

    Error Node::serialise(BinaryOutFileAccessor file) const noexcept {
        // TODO: serialise process enabled
        if (const Error error = Object::serialise(file); error != Error::none)
            return error;

        const std::string_view name = mName == getClass() ? std::string_view{} : mName;

        if (name.size() > 1024)
            return Error::file_invalid_data;

        if (
            !file.write(visible) ||
            !file.write(static_cast<std::uint16_t>(name.size())) ||
            !file.write(std::span{name})
        )
            return Error::file_write_fail;

        const auto childCountPos = file.tell();

        std::uint16_t childCount {};

        if (!file.write(static_cast<std::uint16_t>(0)))
            return Error::file_write_fail;

        for (const std::unique_ptr<Node>& child : mChildren) {
            if (child->isHelper()) continue;

            ++childCount;
            if (const Error error = child->serialise(file); error != Error::none)
                return error;
        }

        if (childCount > 1024)
            return Error::file_invalid_data;

        if (
            !file.seek(childCountPos) ||
            !file.write(childCount) ||
            !file.seek(0, std::ios::end)
        )
            return Error::file_write_fail;

        return Error::none;
    }

    Error Node::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Object::deserialise(file); error != Error::none)
            return error;

        if (!file.read(visible))
            return Error::file_read_fail;

        std::uint16_t nameLength;
        if (!file.read(nameLength))
            return Error::file_read_fail;

        if (nameLength > 1024)
            return Error::file_invalid_data;

        mName.resize(nameLength);
        if (!file.read(std::span{mName}))
            return Error::file_read_fail;

        std::uint16_t childCount;
        if (!file.read(childCount))
            return Error::file_read_fail;

        mChildren.reserve(mChildren.size() + childCount);

        for (std::uint16_t i{}; i < childCount; ++i) {
            if (std::expected exp = Registry::deserialise(file))
                emplaceChild(object_pointer_cast<Node>(std::move(exp.value())));
            else
                return exp.error();
        }

        return Error::none;
    }

    const CanvasLayer* Node::getCanvasLayer() const noexcept {
        return mCanvasLayer;
    }

    Viewport* Node::getViewport() noexcept {
        return mViewport;
    }

    const Viewport* Node::getViewport() const noexcept {
        return mViewport;
    }
}
