#pragma once

#ifdef __3DS__
#include <3ds/gfx.h>
#include <3ds/gfx.h>

#include <m3ds/render/DVLB.hpp>
#include <m3ds/render/Light.hpp>
#include <m3ds/render/LightEnvironment.hpp>
#include <m3ds/render/ShaderProgram.hpp>
#endif

#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Transform2D.hpp>
#include <m3ds/spatial/Matrix4x4.hpp>

#include <m3ds/utils/Texture.hpp>
#include <m3ds/types/Style.hpp>
#include <m3ds/reference/resource/Font.hpp>

#ifdef M3DS_SFML
#include <SFML/Window/Window.hpp>
#include <GL/glew.h>
#endif

namespace M3DS {

    class MeshInstance;

#ifdef __3DS__
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
#endif

    class RenderTarget {
        friend class Viewport;
        friend class CanvasLayer;
    public:
#ifdef __3DS__
        enum class ClearFlags : std::uint32_t {
            colour = 1,
            depth = 1 << 1,
            all = colour | depth
        };
#elifdef M3DS_SFML
        enum class ClearFlags : std::uint32_t {
            colour = GL_CLEAR_BUFFER,
            depth = GL_DEPTH_BUFFER_BIT,
            all = colour | depth
        };
#endif

#ifdef __3DS__
        explicit RenderTarget(Screen screen = Screen::top, bool stereoscopic3d = true) noexcept;

        [[nodiscard]] Screen getScreen() const noexcept;

        [[nodiscard]] C3D_RenderTarget* getLeft() noexcept;
        [[nodiscard]] const C3D_RenderTarget* getLeft() const noexcept;

        [[nodiscard]] C3D_RenderTarget* getRight() noexcept;
        [[nodiscard]] const C3D_RenderTarget* getRight() const noexcept;
#elifdef M3DS_SFML
        explicit RenderTarget(sf::Window window) noexcept;

        sf::Window* get() noexcept;
#endif
        [[nodiscard]] Vector2i getSize() const;

        void clear(Colour colour, ClearFlags flags = ClearFlags::all) noexcept;

    private:
#ifdef __3DS__
        std::unique_ptr<C3D_RenderTarget, RenderTargetDeleter> mTargetLeft {};
        std::unique_ptr<C3D_RenderTarget, RenderTargetDeleter> mTargetRight {};
        Screen mScreen {};
#elifdef M3DS_SFML
        sf::Window mWindow {};
#endif
    };

// TODO: Cleanup of RenderTarget2D

    class RenderTarget2D {
    public:
#ifdef __3DS__
        explicit RenderTarget2D(C3D_RenderTarget* target) noexcept;

        void drawImage(
            const C2D_Image& image,
            const Transform2D& transform,
            bool centre = true
        );
#elifdef M3DS_SFML
        explicit RenderTarget2D(sf::Window* window) noexcept;
#endif

        void drawTextureFrame(
            const Texture& texture,
            const Transform2D& transform,
            std::uint32_t frame,
            bool centre = true
        );

        void drawTexture(
            const Texture& texture,
            const Transform2D& transform,
            bool centre = true
        );

        void drawTextureWithSize(
            const Texture& texture,
            const Transform2D& transform,
            const Vector2& size,
            std::uint32_t frame = 0,
            bool centre = true
        );

#ifdef __3DS__
        void drawTexture(
            C3D_Tex* texture,
            const Transform2D& transform
        );
#endif

        void drawTextureSection(
            const Texture& texture,
            const Transform2D& transform,
            const Rect2& drawSection,
            bool centre = true
        );

        void drawRectSolid(
            const Transform2D& transform,
            const Vector2& rectangleSize,
            Colour colour = Colours::green
        );

        void drawRect(
            const Transform2D& transform,
            const Vector2& rectangleSize,
            Colour colour = Colours::green
        );

        void drawRect(
            const Transform2D& transform,
            const Vector2& rectangleSize,
            std::span<Colour, 4> colours
        );

        void drawCircle(
            const Transform2D& transform,
            float radius,
            Colour colour = Colours::green
        );

        void drawChar(
            const Transform2D& transform,
            const Font& font,
            char c,
            Colour colour = Colours::green
        );

        void drawChar(
            const Transform2D& transform,
            const Font& font,
            const Font::Glyph& glyph,
            Colour colour = Colours::green
        );

        void prepare() noexcept;

        [[nodiscard]] Vector2i getSize() const;

        void clear() noexcept;

        void draw(const Style& style, const Transform2D& transform, const Vector2& boxSize);
        void draw(const BoxStyle& style, const Transform2D& transform, const Vector2& boxSize);
        void draw(const TextureStyle& style, const Transform2D& transform, const Vector2& boxSize);

        void clearCamera() noexcept;
        void setCameraPos(const Vector2& position) noexcept;
        [[nodiscard]] const std::optional<Vector2>& getCameraPos() const noexcept;

        void clearScissor() noexcept;
        void setScissor(const Rect2& rect2) noexcept;
        [[nodiscard]] const std::optional<Rect2>& getScissor() const noexcept;

    private:
#ifdef __3DS__
        C3D_RenderTarget* mTarget;
#elifdef M3DS_SFML
        sf::Window* mWindow {};
#endif

        std::optional<Vector2> mCameraPos {};
        std::optional<Rect2> mScissor {};
    };

    class RenderTarget3D {
    public:
#ifdef __3DS__
        explicit RenderTarget3D(C3D_RenderTarget* target, LightEnv& lightEnv) noexcept;
#elifdef M3DS_SFML
        explicit RenderTarget3D(sf::Window* window) noexcept;
#endif

        void clear() noexcept;
        
#ifdef __3DS__
        void prepare(float iod) noexcept;
#elifdef M3DS_SFML
        void prepare() noexcept;
#endif
        void render(const MeshInstance& meshInstance) noexcept;

        void setCameraPos(const Matrix4x4& transform) noexcept;
    private:
#ifdef __3DS__
        static ShaderProgram program;

        // Might wanna move this?
        static std::int8_t uLoc_projection;
        static std::int8_t uLoc_modelView;
        static std::int8_t uLoc_bones;

        C3D_RenderTarget* mTarget;
        LightEnv& mLightEnv;
#elifdef M3DS_SFML
        static inline ShaderProgram program { vShader, fShader };
        sf::Window* mWindow {};
#endif

        Matrix4x4 mCameraInverse = Matrix4x4::identity();
        Matrix4x4 mProjection = Matrix4x4::identity();
    };
// #elifdef M3DS_SFML
//     class RenderTarget {
//         friend class Viewport;
//         friend class CanvasLayer;
//     public:
//         enum class ClearFlags : std::uint32_t {
//             colour = 1,
//             depth = 1 << 1,
//             all = colour | depth
//         };
//
//         explicit RenderTarget(const Vector2i& size = { 1280, 720 }) noexcept;
//         ~RenderTarget() noexcept;
//
//         void clear(Colour colour, ClearFlags flags = ClearFlags::all) noexcept;
//     };
//
//     class RenderTarget2D {
//         RenderTarget& mTarget;
//     public:
//         explicit RenderTarget2D(RenderTarget& target) noexcept;
//         // void drawImage(
//         //     const C2D_Image& image,
//         //     const Transform2D& transform,
//         //     bool centre = true
//         // );
//
//         void drawTextureFrame(
//             const Texture& texture,
//             const Transform2D& transform,
//             std::uint32_t frame,
//             bool centre = true
//         );
//
//         void drawTexture(
//             const Texture& texture,
//             const Transform2D& transform,
//             bool centre = true
//         );
//
//         // void drawTexture(
//         //     C3D_Tex* texture,
//         //     const Transform2D& transform
//         // );
//
//         void drawTextureSection(
//             const Texture& texture,
//             const Transform2D& transform,
//             const Rect2& drawSection,
//             bool centre = true
//         );
//
//         void drawBoxStyle(
//             const BoxStyle& style,
//             const Transform2D& transform,
//             Vector2 boxSize
//         );
//
//         void drawRectSolid(
//             const Transform2D& transform,
//             const Vector2& rectangleSize,
//             Colour colour = Colours::green
//         );
//
//         void drawRect(
//             const Transform2D& transform,
//             const Vector2& rectangleSize,
//             Colour colour = Colours::green
//         );
//
//         void drawRect(
//             const Transform2D& transform,
//             const Vector2& rectangleSize,
//             std::span<Colour, 4> colours
//         );
//
//         void drawCircle(
//             const Transform2D& transform,
//             float radius,
//             Colour colour = Colours::green
//         );
//
//         // void drawChar(
//         //     const Transform2D& transform,
//         //     const Font& font,
//         //     char c,
//         //     Colour colour = Colours::green
//         // );
//
//         // void drawChar(
//         //     const Transform2D& transform,
//         //     const Font& font,
//         //     const Glyph& glyph,
//         //     Colour colour = Colours::green
//         // );
//
//         void prepare() noexcept;
//
//         [[nodiscard]] Vector2i getSize() const;
//
//         void drawBoxStyle(const BoxStyle& style, const Transform2D& transform, Vector2 boxSize) const;
//
//         void setCameraPos(const Vector2& position) noexcept;
//     };
//     class RenderTarget3D {
//         RenderTarget& mTarget;
//     public:
//         explicit RenderTarget3D(RenderTarget& target) noexcept;
//
//         void prepare() noexcept;
//
//         void render(const MeshInstance& meshInstance) noexcept;
//
//         void setCameraPos(const Matrix4x4& matrix) noexcept;
//     };
// #endif
}
