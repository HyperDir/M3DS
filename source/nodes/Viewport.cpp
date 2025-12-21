#include <m3ds/nodes/Viewport.hpp>

#include <stack>

#include <m3ds/nodes/Root.hpp>
#include <m3ds/nodes/2d/CollisionObject2D.hpp>
#include <m3ds/nodes/3d/CollisionObject3D.hpp>

namespace M3DS {
    Viewport::Viewport(RenderTarget target) noexcept
        : mTarget(std::move(target))
    {
        mViewport = this;
    }

#ifdef __3DS__
    Viewport::Viewport(const Screen screen) noexcept
        : mTarget(screen)
    {
        mViewport = this;
    }
#elifdef M3DS_SFML
    Viewport::Viewport(sf::Window window) noexcept
        : mTarget(std::move(window))
    {
        mViewport = this;
    }
#endif

    void Viewport::notification(const Notification notification) {
        Node::notification(notification);
    }

    void Viewport::afterTreeEnter() {
        Node::afterTreeEnter();

        mRoot->addViewport(this);
    }

    void Viewport::beforeTreeExit() {
        Node::beforeTreeExit();

        mRoot->removeViewport(this);
    }


    void Viewport::physicsUpdate(const Seconds<float> delta) noexcept {
        mPhysicsServer3D.updateAreas();
        mPhysicsServer3D.step(delta);

        for (const auto& area : mPhysicsServer3D.getAreas())
            static_cast<CollisionObject3D*>(area.userData)->readback();

        for (const auto& body : mPhysicsServer3D.getKinematicBodies())
            static_cast<CollisionObject3D*>(body.userData)->readback();

        mPhysicsServer2D.updateAreas();
        mPhysicsServer2D.step(delta);

        for (const auto& area : mPhysicsServer2D.getAreas())
            static_cast<CollisionObject2D*>(area.userData)->readback();

        for (const auto& body : mPhysicsServer2D.getKinematicBodies())
            static_cast<CollisionObject2D*>(body.userData)->readback();
    }

    void Viewport::display() noexcept {
#ifdef M3DS_SFML
        mTarget.mWindow.display();
#endif
    }
    
    void Viewport::setCamera2d(Camera2D* camera) noexcept {
        mCamera2D = camera;
    }

    void Viewport::setCamera3d(Camera3D* camera) noexcept {
        mCamera3D = camera;
    }

    Camera2D* Viewport::getCamera2D() noexcept {
        return mCamera2D;
    }

    const Camera2D* Viewport::getCamera2D() const noexcept {
        return mCamera2D;
    }

    Camera3D* Viewport::getCamera3D() noexcept {
        return mCamera3D;
    }

    const Camera3D* Viewport::getCamera3D() const noexcept {
        return mCamera3D;
    }

    void Viewport::clear(const Colour colour, const RenderTarget::ClearFlags flags) noexcept {
        mTarget.clear(colour, flags);
    }

    void Viewport::treeDraw2D() noexcept {
#ifdef __3DS__
        RenderTarget2D target2d { mTarget.getLeft() };
#elifdef M3DS_SFML
        RenderTarget2D target2d { mTarget.get() };
#endif
        target2d.prepare();

        if (mCamera2D)
            target2d.setCameraPos(mCamera2D->getGlobalTransform().position);

        draw(target2d);
    }

    void Viewport::treeDraw3D() noexcept {
#ifdef __3DS__
        const float iod = osGet3DSliderState() / 3;

        for (unsigned int i{}; i < mLights.size(); ++i) {
            const auto& light = mLights[i];

            auto& internal = mLightEnv.mLights[i];
            if (light) {
                internal.setColour(light->colour.x, light->colour.y, light->colour.z);
                internal.setPosition(light->getGlobalTranslation());
            } else {
                internal.setColour(0, 0, 0);
            }
        }
        mLightEnv.bind();

        {
            RenderTarget3D targetLeft { mTarget.getLeft(), mLightEnv };

            targetLeft.prepare(-iod);
            if (mCamera3D)
                targetLeft.setCameraPos(mCamera3D->getGlobalTransform());

            draw(targetLeft);
        }

        if (C3D_RenderTarget* rTarget = mTarget.getRight(); rTarget && iod > 0) {
            RenderTarget3D targetRight { rTarget, mLightEnv };
            targetRight.prepare(iod);
            if (mCamera3D)
                targetRight.setCameraPos(mCamera3D->getGlobalTransform());

            draw(targetRight);
        }
#elifdef M3DS_SFML
        sf::Window* window = mTarget.get();
        RenderTarget3D target3d { window };

        while (const auto event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>())
                propagateNotification<false>(Notification::exit);
        }
        target3d.prepare();
        if (mCamera3D)
            target3d.setCameraPos(mCamera3D->getGlobalTransform());
        draw(target3d);
#endif
    }

    bool Viewport::addLight(const Light3D* light) noexcept {
        for (auto& internalLight : mLights) {
            if (!internalLight) {
                internalLight = light;
                return true;
            }
        }
        return false;
    }

    void Viewport::removeLight(const Light3D* light) noexcept {
        for (auto& mLight : mLights) {
            if (mLight == light) {
                mLight = nullptr;
                break;
            }
        }
    }

    bool Viewport::isLightActive(const Light3D* light) const noexcept {
        return std::ranges::contains(mLights, light);
    }

    Vector2i Viewport::getSize() const noexcept {
        return mTarget.getSize();
    }

    Error Viewport::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error Viewport::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_METHODS(
        Viewport,
        MUTABLE_METHOD(setCamera2d),
        MUTABLE_METHOD(setCamera3d),
        MUTABLE_METHOD(treeDraw2D),
        MUTABLE_METHOD(treeDraw3D),
        MUTABLE_METHOD(addLight),
        MUTABLE_METHOD(removeLight),
        CONST_METHOD(getSize)
    );

    REGISTER_NO_MEMBERS(Viewport);
}
