#include <m3ds/nodes/Root.hpp>

#include <stack>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    Root::Root() noexcept {
        mName = "Root";
        mRoot = this;
    }

    void Root::treeInput() noexcept {
        std::array<Input::InputFrame, 8> inputFrames;
        std::size_t inputFrameCount {};

        for (std::size_t i{}; i < inputFrames.size(); ++i) {
            const Input::ControllerState& state = Input::getControllerState(i);

            if (state)
                inputFrames[inputFrameCount++] = Input::InputFrame{state};
        }



        const auto inputHelper = [&](this auto&& self, Node* node) -> void {
            for (const auto& child : node->getChildren() | std::views::reverse)
                self(child.get());

            for (Input::InputFrame& inputFrame : std::span{inputFrames.begin(), inputFrameCount}) {
                if (const auto script = node->getScript())
                    script->input(inputFrame);

                node->input(inputFrame);
            }
        };

        inputHelper(this);
    }

    std::span<Viewport* const> Root::getViewports() noexcept {
        return mViewports;
    }

    std::span<const Viewport* const> Root::getViewports() const noexcept {
        return mViewports;
    }

    void Root::notification(const Notification notification) {
        Node::notification(notification);

        if (notification == Notification::exit)
            mExit = true;
    }

    void Root::exit() noexcept {
        mExit = true;
    }

    void Root::mainLoop() noexcept {
        mainLoop([&](const float delta) {
            if (M3DS::Input::isKeyPressed(Input::Key::start))
                exit();

            treeInput();
            treeUpdate(delta);
            treeDraw();
        });
    }

    void Root::addViewport(Viewport* viewport) {
        mViewports.emplace_back(viewport);
    }

    void Root::removeViewport(Viewport* viewport) {
        std::erase(mViewports, viewport);
    }

    void Root::enableUpdate(Node* node) {
        mUpdateList.emplace_back(node);
    }

    void Root::disableUpdate(Node* node) {
        std::erase(mUpdateList, node);
    }

    Error Root::serialise([[maybe_unused]] BinaryOutFileAccessor file) const noexcept {
        return Error::root_serialisation_disabled;
    }

    Error Root::deserialise([[maybe_unused]] BinaryInFileAccessor file) noexcept {
        return Error::root_serialisation_disabled;
    }

    void Root::treeUpdate(const Seconds<float> delta) noexcept {
        // std::stack<Node*, std::vector<Node*>> stack {};
        // stack.emplace(this);
        //
        // while (!stack.empty()) {
        //     Node* curr = stack.top();
        //     stack.pop();
        //
        //     curr->update(delta);
        //     if (auto* const script = curr->getScript())
        //         script->process(delta);
        //
        //     for (auto& child : curr->getChildren()) {
        //         stack.emplace(child.get());
        //     }
        // }
        //

        for (Node* node : mUpdateList) {
            node->update(delta);
            if (node->mScript)
                node->mScript->update(delta);
        }

        for (auto* viewport : mViewports) {
            viewport->physicsUpdate(delta);
        }
    }

    void Root::treeDraw(const Draw draw) noexcept {
        for (Viewport* viewport : getViewports()) {
            viewport->clear();
            if (draw & Draw::draw_3d)
                viewport->treeDraw3D();
            if (draw & Draw::draw_2d)
                viewport->treeDraw2D();

            viewport->display();
        }
    }

    REGISTER_METHODS(
        Root,

        MUTABLE_METHOD(treeUpdate),
        MUTABLE_METHOD(treeInput),
        MUTABLE_METHOD(exit),
        MUTABLE_METHOD(enableUpdate),
        MUTABLE_METHOD(disableUpdate)
    );

    REGISTER_NO_MEMBERS(Root);
}
