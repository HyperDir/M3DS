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

#include <m3ds/containers/StaticHeapArray.hpp>

namespace M3DS {
    class MeshInstance;

    enum class Screen : std::uint8_t {
        top,
        bottom
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

    template <typename Element, std::size_t N, typename Allocator>
    class ScratchBuffer {
    public:
        [[nodiscard]] constexpr std::span<Element> getCurrentSpan() noexcept {
            return { mHead, mCount };
        }

        constexpr void startNewSpan() noexcept {
            mHead += mCount;
            mCount = 0;
        }

        template <typename... Args>
        [[nodiscard]] constexpr Failure emplace(Args&&... args) noexcept {
            if (mHead + mCount + 1 >= getCurrentBuffer().end())
                return { ErrorCode::out_of_bounds };

            mHead[mCount++] = Element{ std::forward<Args>(args)... };
            return Success;
        }

        [[nodiscard]] constexpr Failure emplace(std::span<const Element> elements) noexcept {
            if (elements.size() + mHead + mCount >= getCurrentBuffer().end())
                return { ErrorCode::out_of_bounds };

            for (const auto& element : elements)
                mHead[mCount++] = element;
            return Success;
        }

        constexpr void swap() noexcept {
            mCurrentBuffer = !mCurrentBuffer;
            mHead = getCurrentBuffer().data();
            mCount = 0;
        }
    private:
        [[nodiscard]] constexpr StaticHeapArray<Element, N, Allocator>& getCurrentBuffer() noexcept {
            return mBuffers[mCurrentBuffer];
        }

        std::array<StaticHeapArray<Element, N, Allocator>, 2> mBuffers {};
        bool mCurrentBuffer {};
        Element* mHead = getCurrentBuffer().data();
        std::size_t mCount {};
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

        void clear() noexcept;

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

        static std::int8_t uLoc_projection;
        static std::int8_t uLoc_modelView;

        static inline ScratchBuffer<Vertex2D, 1024 * 1024 / sizeof(Vertex2D), LinearAllocator<Vertex2D>> mScratchBuffer {};

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

        void clear() noexcept;

        void prepare(float iod) noexcept;
        void render(const MeshInstance& meshInstance) noexcept;

        void setCameraPos(const Matrix4x4& transform) noexcept;
    private:
        static ShaderProgram program;

        // Might wanna move this?
        static std::int8_t uLoc_projection;
        static std::int8_t uLoc_modelView;
        static std::int8_t uLoc_bones;

        C3D_RenderTarget* mTarget;
        LightEnv& mLightEnv;

        Matrix4x4 mCameraInverse = Matrix4x4::identity();
        Matrix4x4 mProjection = Matrix4x4::identity();
    };
}
