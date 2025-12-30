#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <string>

#include <m3ds/reference/Object.hpp>
#include <m3ds/reference/Script.hpp>

#include <m3ds/types/Notification.hpp>
#include <m3ds/types/NodePath.hpp>

#include <m3ds/utils/Memory.hpp>

namespace M3DS {
    class Root;
    class CanvasLayer;
    class Viewport;
    class Frame;

    class Node : public Object {
        M_CLASS(Node, Object)
        friend class Root;
        friend class CanvasLayer;
        friend class Viewport;
    public:
        bool visible = true;
        // bool paused = false;

        [[nodiscard]] std::span<const std::unique_ptr<Node>> getChildren() noexcept;

        template <std::derived_from<Node> NodeType>
        [[nodiscard]] auto getChildrenOfType() noexcept;

        [[nodiscard]] Node() noexcept = default;

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;

        [[nodiscard]] Node* getParent() noexcept;
        [[nodiscard]] const Node* getParent() const noexcept;

        [[nodiscard]] Root* getRoot() noexcept;
        [[nodiscard]] const Root* getRoot() const noexcept;
        [[nodiscard]] bool isInTree() const noexcept;

        [[nodiscard]] Viewport* getViewport() noexcept;
        [[nodiscard]] const Viewport* getViewport() const noexcept;

        [[nodiscard]] const CanvasLayer* getCanvasLayer() const noexcept;

        void setName(std::string_view name);
        [[nodiscard]] std::string_view getName() const noexcept;

        template <std::derived_from<Node> NodeType, bool isHelper = false, typename... Args>
        NodeType* emplaceChild(Args&&... args);

        Node* emplaceChild(std::unique_ptr<Node> node);
        template <std::derived_from<Node> T>
        T* emplaceChild(std::unique_ptr<T> node);

        template <script_type ScriptType, typename... Args>
        ScriptType* emplaceChild(Args&&... args);

        [[nodiscard]] std::unique_ptr<Node> removeChild(const Node* child) noexcept;
        [[nodiscard]] std::unique_ptr<Node> removeChild(std::size_t idx) noexcept;

        [[nodiscard]] bool isHelper() const noexcept;

        [[nodiscard]] BaseScript* getScript() noexcept;
        [[nodiscard]] const BaseScript* getScript() const noexcept;

        [[nodiscard]] Node* getNode(const NodePath& path) noexcept;
        [[nodiscard]] const Node* getNode(const NodePath& path) const noexcept;

        [[nodiscard]] Node* getChild(std::size_t idx) noexcept;
        [[nodiscard]] const Node* getChild(std::size_t idx) const noexcept;

        [[nodiscard]] std::size_t getChildCount() const noexcept;

        [[nodiscard]] NodePath getPath() const;
        [[nodiscard]] NodePath getPathTo(const Node* other) const;

        void printTree() const noexcept;

        template <bool PropagateDown = true>
        void propagateNotification(Notification notification) noexcept;

        void queueFree();
        void free();
    protected:
        virtual void update(Seconds<float> delta);
        virtual void draw(RenderTarget2D& target);
        virtual void draw(RenderTarget3D& target);
        virtual void input(Input::InputFrame& inputFrame);

        virtual void notification(Notification notification);

        virtual void afterTreeEnter();
        virtual void beforeTreeExit();
    private:
        bool mHelper {};

        std::string mName {};

        Node* mParent {};
        Root* mRoot {};
        Viewport* mViewport {};
        const CanvasLayer* mCanvasLayer {};
        std::vector<std::unique_ptr<Node>> mChildren {};

        std::unique_ptr<BaseScript> mScript {};
    };

    template <std::derived_from<Node> NodeType, bool isHelper, typename... Args>
    NodeType* Node::emplaceChild(Args&&... args) {
        std::unique_ptr childUPtr = M3DS::make_unique_nothrow<NodeType>(std::forward<Args>(args)...);
        if (!childUPtr) {
            Debug::err("Failed to allocate Node!");
            return {};
        }

        auto child = static_cast<NodeType*>(mChildren.emplace_back(std::move(childUPtr)).get());
        child->mParent = this;
        child->mName = NodeType::getClassStatic();
        child->mHelper = isHelper;
        if (isInTree())
            static_cast<Node*>(child)->notification(Notification::tree_entered);
        return child;
    }

    template <std::derived_from<Node> T>
    T* Node::emplaceChild(std::unique_ptr<T> node) {
        return static_cast<T*>(emplaceChild(static_cast<std::unique_ptr<Node>>(std::move(node))));
    }

    template <script_type ScriptType, typename... Args>
    ScriptType* Node::emplaceChild(Args&&... args) {
        using NodeType = ScriptType::NodeType;

        std::unique_ptr childUPtr = M3DS::make_unique_nothrow<NodeType>(std::forward<Args>(args)...);
        if (!childUPtr) {
            Debug::err("Failed to allocate Node!");
            return {};
        }

        auto child = static_cast<NodeType*>(mChildren.emplace_back(std::move(childUPtr)).get());
        child->mParent = this;
        child->mName = NodeType::getClassStatic();

        auto script = M3DS::make_unique_nothrow<ScriptType>();
        if (!script) {
            Debug::err("Failed to allocate Script!");
            return {};
        }
        script->mNode = child;
        child->mScript = std::move(script);
        child->mScript->ready();

        if (isInTree())
            static_cast<Node*>(child)->notification(Notification::tree_entered);
        return static_cast<ScriptType*>(child->mScript.get());
    }

    template <bool PropagateDown>
    void Node::propagateNotification(const Notification notification) noexcept {
        if constexpr (PropagateDown) {
            std::queue<Node*> queue {};
            queue.emplace(this);

            while (!queue.empty()) {
                Node* curr = queue.front();
                queue.pop();

                curr->notification(notification);

                for (const auto& child : curr->getChildren())
                    queue.emplace(child.get());
            }
        } else {
            Node* curr = this;
            while (curr != nullptr) {
                curr->notification(notification);
                curr = curr->mParent;
            }
        }
    }

    template <std::derived_from<Node> NodeType>
    auto Node::getChildrenOfType() noexcept {
        return mChildren
            | std::views::transform([](const auto& child) { return object_cast<NodeType*>(child.get()); })
            | std::views::filter([](const auto& child) { return child != nullptr; });
    }
}
