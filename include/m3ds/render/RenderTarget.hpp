#pragma once

#include <m3ds/render/Light.hpp>
#include <m3ds/render/LightEnvironment.hpp>
#include <m3ds/render/ShaderProgram.hpp>

#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Transform2D.hpp>
#include <m3ds/spatial/Matrix4x4.hpp>

#include <m3ds/render/SpriteSheet.hpp>
#include <m3ds/types/Style.hpp>
#include <m3ds/reference/resource/Font.hpp>

#include <m3ds/render/Mesh2D.hpp>

namespace M3DS {
    class MeshInstance;

    enum class Screen : std::uint8_t {
        top,
        bottom
    };

    class DrawEnvironment {
        friend class Root;

        DrawEnvironment() noexcept;
        ~DrawEnvironment() noexcept;
    };

    struct RenderTargetDeleter {
        void operator()(C3D_RenderTarget* target) const noexcept {
            if (target)
                C3D_RenderTargetDelete(target);
        }
    };

    class RenderTarget {
        friend class Viewport;
        friend class CanvasLayer;
    public:
        enum class ClearFlags : std::uint32_t {
            colour = 1,
            depth = 1 << 1,
            all = colour | depth
        };

        explicit RenderTarget(Screen screen = Screen::top, bool stereoscopic3d = true) noexcept;

        [[nodiscard]] Screen getScreen() const noexcept;

        [[nodiscard]] C3D_RenderTarget* getLeft() noexcept;
        [[nodiscard]] const C3D_RenderTarget* getLeft() const noexcept;

        [[nodiscard]] C3D_RenderTarget* getRight() noexcept;
        [[nodiscard]] const C3D_RenderTarget* getRight() const noexcept;

        [[nodiscard]] Vector2i getSize() const;

        void clear(Colour colour, ClearFlags flags = ClearFlags::all) noexcept;
    private:
        std::unique_ptr<C3D_RenderTarget, RenderTargetDeleter> mTargetLeft {};
        std::unique_ptr<C3D_RenderTarget, RenderTargetDeleter> mTargetRight {};
        Screen mScreen {};
    };

    class RenderTarget2D {
    public:
        explicit RenderTarget2D(C3D_RenderTarget* target) noexcept;

        ~RenderTarget2D() noexcept;

        void draw(const Triangle2D& triangle, C3D_Tex* texture = nullptr) noexcept;
        void draw(const Quad2D& quad, C3D_Tex* texture = nullptr) noexcept;

        void draw(const Mesh2D& mesh) noexcept;

        void drawTextureFrame(
            const SpriteSheet& spriteSheet,
            const Transform2D& transform,
            uint32_t frame,
            bool centre = false
        ) noexcept;

        void draw(const Style& style, const Transform2D& transform, const Vector2& boxSize);
        void draw(const BoxStyle& style, const Transform2D& transform, const Vector2& boxSize);
        void draw(const TextureStyle& style, const Transform2D& transform, const Vector2& boxSize);

        void drawRectSolid(const Transform2D& transform, Vector2 size, Colour colour);

        void prepare() noexcept;

        [[nodiscard]] Vector2i getSize() const;

        void clearCamera() noexcept;
        void setCameraPos(const Vector2& position) noexcept;
        [[nodiscard]] const std::optional<Vector2>& getCameraPos() const noexcept;

        void clearScissor() noexcept;
        void setScissor(const Rect2& rect2) noexcept;
        [[nodiscard]] const std::optional<Rect2>& getScissor() const noexcept;
    private:
        void enablePrimitive() noexcept;
        void disablePrimitive() noexcept;

        void primitiveSendVertex(const Vertex2D& vertex) noexcept;

        void primitiveFlush() noexcept;

        void bind(C3D_Tex* texture) noexcept;
        void setTint(Colour colour) noexcept;


        static ShaderProgram program;
        static const struct Uniforms {
            std::int8_t projection {};
            std::int8_t modelView {};
        } uniforms;

        C3D_RenderTarget* mTarget;

        Matrix4x4 mProjection = Matrix4x4::orthoTilt<false>(0.0, 400.0, 240.0, 0.0, 0.0, 1.0);
        Matrix4x4 mCameraInverse = Matrix4x4::identity();

        C3D_Tex* mBoundTexture {};
        Colour mTintColour { 0xFF, 0xFF, 0xFF, 0xFF };

        std::optional<Vector2> mCameraPos {};
        std::optional<Rect2> mScissor {};

        bool mPrimitive {};
    };

    class RenderTarget3D {
    public:
        explicit RenderTarget3D(C3D_RenderTarget* target, LightEnv& lightEnv) noexcept;

        void prepare(float iod) noexcept;
        void render(const MeshInstance& meshInstance) noexcept;

        void setCameraPos(const Matrix4x4& transform) noexcept;
    private:
        static ShaderProgram program;
        static const struct Uniforms {
            std::int8_t projection {};
            std::int8_t modelView {};
            std::int8_t bones {};
        } uniforms;

        C3D_RenderTarget* mTarget;
        LightEnv& mLightEnv;

        Matrix4x4 mCameraInverse = Matrix4x4::identity();
        Matrix4x4 mProjection = Matrix4x4::identity();
    };
}
