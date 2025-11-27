#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/nodes/3d/Light3D.hpp>
#include <m3ds/nodes/3d/Camera3D.hpp>
#include <m3ds/nodes/2d/Camera2D.hpp>

#include <m3ds/utils/Frame.hpp>
#include <m3ds/lib/SPhys/SPhys.hpp>

#ifdef M3DS_SFML
#include <SFML/Window/VideoMode.hpp>
#endif

namespace M3DS {
    class Viewport : public Node {
        M_CLASS(Viewport, Node)

        friend class CollisionObject2D;
        friend class CollisionObject3D;
        friend class Root;

    public:
        explicit Viewport(RenderTarget target) noexcept;
#ifdef __3DS__
        explicit Viewport(Screen screen = Screen::top) noexcept;
#elifdef M3DS_SFML
        explicit Viewport(sf::Window window = { sf::VideoMode({800, 600}), "M3DS" }) noexcept;
#endif

        void setCamera2d(Camera2D* camera) noexcept;
        [[nodiscard]] Camera2D* getCamera2D() noexcept;
        [[nodiscard]] const Camera2D* getCamera2D() const noexcept;

        void setCamera3d(Camera3D* camera) noexcept;
        [[nodiscard]] Camera3D* getCamera3D() noexcept;
        [[nodiscard]] const Camera3D* getCamera3D() const noexcept;

        void clear(
            Colour colour = Colours::black,
            RenderTarget::ClearFlags flags = RenderTarget::ClearFlags::all
        ) noexcept;

        void treeDraw2D() noexcept;
        void treeDraw3D() noexcept;

        bool addLight(const Light3D* light) noexcept;
        void removeLight(const Light3D* light) noexcept;
        [[nodiscard]] bool isLightActive(const Light3D* light) const noexcept;

        [[nodiscard]] Vector2i getSize() const noexcept;

        [[nodiscard]] constexpr auto& getPhysicsServer3d() noexcept;
        [[nodiscard]] constexpr const auto& getPhysicsServer3d() const noexcept;

        [[nodiscard]] constexpr auto& getPhysicsServer2d() noexcept;
        [[nodiscard]] constexpr const auto& getPhysicsServer2d() const noexcept;
    protected:
        void notification(Notification notification) override;

        void afterTreeEnter() override;
        void beforeTreeExit() override;

        void physicsUpdate(Seconds<float> delta) noexcept;

        void display() noexcept;
    private:
        RenderTarget mTarget;
        Camera2D* mCamera2D {};
        Camera3D* mCamera3D {};

#ifdef __3DS__
        LightEnv mLightEnv {};
#endif
        std::array<const Light3D*, 8> mLights {};

        SPhys::PhysicsServer3D<> mPhysicsServer3D {};
        SPhys::PhysicsServer2D<> mPhysicsServer2D {};
    };

    constexpr auto& Viewport::getPhysicsServer3d() noexcept {
        return mPhysicsServer3D;
    }

    constexpr const auto& Viewport::getPhysicsServer3d() const noexcept {
        return mPhysicsServer3D;
    }

    constexpr auto& Viewport::getPhysicsServer2d() noexcept {
        return mPhysicsServer2D;
    }

    constexpr const auto& Viewport::getPhysicsServer2d() const noexcept {
        return mPhysicsServer2D;
    }
}
