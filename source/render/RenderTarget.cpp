// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
#include <m3ds/render/RenderTarget.hpp>

#include <cmath>

#include <m3ds/nodes/3d/MeshInstance.hpp>

#include <cstring>

constinit std::array vshader = std::to_array<unsigned char>({
#embed <vshader.bin>
});

#ifdef M3DS_SFML
#include <GL/glew.h>
#endif

namespace M3DS {
#ifdef __3DS__
    ShaderProgram RenderTarget3D::program { std::span{vshader} };
    
    std::int8_t RenderTarget3D::uLoc_projection = program.getUniformLocation("projection");
    std::int8_t RenderTarget3D::uLoc_modelView = program.getUniformLocation("modelView");
    std::int8_t RenderTarget3D::uLoc_bones = program.getUniformLocation("bones");

    RenderTarget::RenderTarget(const Screen screen, const bool stereoscopic3d) noexcept
        : mScreen(screen)
    {
        static constexpr std::uint32_t displayTransferFlags = (
            GX_TRANSFER_FLIP_VERT(0) 						|
            GX_TRANSFER_OUT_TILED(0) 						|
            GX_TRANSFER_RAW_COPY(0) 						|
            GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) 	|
            GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) 	|
            GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO)
        );

        mTargetLeft.reset(C3D_RenderTargetCreate(
            240,
            screen == Screen::top ? GSP_SCREEN_HEIGHT_TOP : GSP_SCREEN_HEIGHT_BOTTOM,
            GPU_RB_RGBA8,
            GPU_RB_DEPTH24_STENCIL8
        ));

        C3D_RenderTargetSetOutput(
            mTargetLeft.get(),
            screen == Screen::top ? GFX_TOP : GFX_BOTTOM,
            GFX_LEFT,
            displayTransferFlags
        );

        if (stereoscopic3d && screen == Screen::top) {
            mTargetRight.reset(C3D_RenderTargetCreate(
                240,
                GSP_SCREEN_HEIGHT_TOP,
                GPU_RB_RGBA8,
                GPU_RB_DEPTH24_STENCIL8
            ));

            C3D_RenderTargetSetOutput(
                mTargetRight.get(),
                GFX_TOP,
                GFX_RIGHT,
                displayTransferFlags
            );
        }
    }

    Screen RenderTarget::getScreen() const noexcept {
        return mScreen;
    }

    Vector2i RenderTarget::getSize() const {
        return { mScreen == Screen::top ? 400 : 320, 240 };
    }

    C3D_RenderTarget* RenderTarget::getLeft() noexcept {
        return mTargetLeft.get();
    }

    const C3D_RenderTarget* RenderTarget::getLeft() const noexcept {
        return mTargetLeft.get();
    }

    C3D_RenderTarget* RenderTarget::getRight() noexcept {
        return mTargetLeft.get();
    }

    const C3D_RenderTarget* RenderTarget::getRight() const noexcept {
        return mTargetLeft.get();
    }

    void RenderTarget::clear(const Colour colour, const ClearFlags flags) noexcept {
        const std::uint32_t c =
            static_cast<std::uint32_t>(colour.r) << 24 |
            static_cast<std::uint32_t>(colour.g << 16) |
            static_cast<std::uint32_t>(colour.b << 8) |
            static_cast<std::uint32_t>(colour.a);

        if (mTargetLeft)
            C3D_RenderTargetClear(
                mTargetLeft.get(),
                static_cast<C3D_ClearBits>(flags),
                c,
                0
            );
        if (mTargetRight)
            C3D_RenderTargetClear(
                mTargetRight.get(),
                static_cast<C3D_ClearBits>(flags),
                c,
                0
            );
    }

    RenderTarget2D::RenderTarget2D(C3D_RenderTarget* target) noexcept : mTarget(target) {}

    Vector2i RenderTarget2D::getSize() const {
        return { mTarget->screen == GFX_TOP ? 400 : 320, 240 };
    }

    void RenderTarget2D::prepare() noexcept {
        C2D_Prepare();
        C2D_SceneBegin(mTarget);
    }

    void RenderTarget2D::clear() noexcept {
        C2D_TargetClear(mTarget, 0);
    }

    void RenderTarget2D::draw(const Style& style, const Transform2D& transform, const Vector2& boxSize) {
        style.visit([&](auto&& s) {
            draw(s, transform, boxSize);
        });
    }

    void RenderTarget2D::draw(
        const BoxStyle& style,
        const Transform2D& transform,
        const Vector2& boxSize
    ) {
        const Vector2& pos = transform.position.round();

        if (style.cornerRadius == 0 || style.cornerDetail == 0) {
            C2D_DrawRectSolid(
                pos.x,
                pos.y,
                0,
                boxSize.x,
                boxSize.y,
                static_cast<std::uint32_t>(style.colour)
            );
            return;
        }
        C2D_DrawTriangle(
            pos.x,
            pos.y + style.cornerRadius - 1,
            static_cast<std::uint32_t>(style.colour),
            pos.x + boxSize.x / 2,
            pos.y + boxSize.y / 2,
            static_cast<std::uint32_t>(style.centreColour),
            pos.x,
            pos.y + boxSize.y - style.cornerRadius + 1,
            static_cast<std::uint32_t>(style.colour),
            0
        );
        C2D_DrawTriangle(
            pos.x + style.cornerRadius,
            pos.y,
            static_cast<std::uint32_t>(style.colour),
            pos.x + boxSize.x / 2,
            pos.y + boxSize.y / 2,
            static_cast<std::uint32_t>(style.centreColour),
            pos.x + boxSize.x - style.cornerRadius,
            pos.y,
            static_cast<std::uint32_t>(style.colour),
            0
        );
        C2D_DrawTriangle(
            pos.x + boxSize.x,
            pos.y + style.cornerRadius - 1,
            static_cast<std::uint32_t>(style.colour),
            pos.x + boxSize.x / 2,
            pos.y + boxSize.y / 2,
            static_cast<std::uint32_t>(style.centreColour),
            pos.x + boxSize.x,
            pos.y + boxSize.y - style.cornerRadius + 1,
            static_cast<std::uint32_t>(style.colour),
            0
        );
        C2D_DrawTriangle(
            pos.x + style.cornerRadius,
            pos.y + boxSize.y,
            static_cast<std::uint32_t>(style.colour),
            pos.x + boxSize.x / 2,
            pos.y + boxSize.y / 2,
            static_cast<std::uint32_t>(style.centreColour),
            pos.x + boxSize.x - style.cornerRadius,
            pos.y + boxSize.y,
            static_cast<std::uint32_t>(style.colour),
            0
        );

        Vector2 prevAngles { style.cornerRadius, 0 };
        const Vector2 centre = Vector2(pos) + boxSize / 2.f;
        for (int i = 1; i <= style.cornerDetail; ++i) {
            const float angle = std::numbers::pi_v<float> / 2.f * static_cast<float>(i) / static_cast<float>(style.cornerDetail);

            const Vector2 angles = Vector2(1-std::sin(angle), 1-std::cos(angle)) * (style.cornerRadius - 1);

            C2D_DrawTriangle(
                pos.x + angles.x,
                pos.y + angles.y,
                static_cast<std::uint32_t>(style.colour),
                centre.x,
                centre.y,
                static_cast<std::uint32_t>(style.centreColour),
                pos.x + prevAngles.x,
                pos.y + prevAngles.y,
                static_cast<std::uint32_t>(style.colour),
                0
            );

            C2D_DrawTriangle(
                pos.x + boxSize.x - angles.x,
                pos.y + angles.y,
                static_cast<std::uint32_t>(style.colour),
                centre.x,
                centre.y,
                static_cast<std::uint32_t>(style.centreColour),
                pos.x + boxSize.x - prevAngles.x,
                pos.y + prevAngles.y,
                static_cast<std::uint32_t>(style.colour),
                0
            );

            C2D_DrawTriangle(
                pos.x + angles.x,
                pos.y + boxSize.y - angles.y,
                static_cast<std::uint32_t>(style.colour),
                centre.x,
                centre.y,
                static_cast<std::uint32_t>(style.centreColour),
                pos.x + prevAngles.x,
                pos.y + boxSize.y - prevAngles.y,
                static_cast<std::uint32_t>(style.colour),
                0
            );

            C2D_DrawTriangle(
                pos.x + boxSize.x - angles.x,
                pos.y + boxSize.y - angles.y,
                static_cast<std::uint32_t>(style.colour),
                centre.x,
                centre.y,
                static_cast<std::uint32_t>(style.centreColour),
                pos.x + boxSize.x - prevAngles.x,
                pos.y + boxSize.y - prevAngles.y,
                static_cast<std::uint32_t>(style.colour),
                0
            );

            prevAngles = angles;
        }
    }

    void RenderTarget2D::draw(const TextureStyle& style, const Transform2D& transform, const Vector2& boxSize) {
        drawTextureWithSize(style.texture, transform, boxSize, style.frame, false);
    }

    void RenderTarget2D::clearCamera() noexcept {
        C2D_ViewReset();
        mCameraPos = {};
    }

    void RenderTarget2D::drawImage(
        const C2D_Image& image,
        const Transform2D& transform,
        const bool centre
    ) {
        const Vector2 scaledSize = Vector2(image.subtex->width, image.subtex->height) * transform.scale;
        const Vector2 centrePos = centre ? scaledSize / 2.f : Vector2(0);

        // if (screenPos.x - scaledSize.x >= static_cast<float>(getSize().x)) return;
        // if (screenPos.x + scaledSize.x < 0) return;
        //
        // if (screenPos.y - scaledSize.y >= static_cast<float>(getSize().y)) return;
        // if (screenPos.y + scaledSize.y < 0) return;

        // if (centre) {
        //     screenPos -= scaledSize / 2.f;
        // }

        const C2D_DrawParams params = {
            {
                std::round(transform.position.x),
                std::round(transform.position.y),

                scaledSize.x,
                scaledSize.y
            },
            {
                centrePos.x,
                centrePos.y
            },
            0,
            transform.rotation
        };

        // Debug::log("Drawing at {} {}", centrePos.x, centrePos.y);
        // Debug::log("UVs: {} {} {} {}", image.subtex->left, image.subtex->top, image.subtex->right, image.subtex->bottom);

        C2D_DrawImage(image, &params);
    }

    void RenderTarget2D::drawTextureFrame(
        const Texture& texture,
        const Transform2D& transform,
        const std::uint32_t frame,
        const bool centre
    ) {
        C2D_Image image = C2D_SpriteSheetGetImage(texture.mRawImage.getNativeHandle(), 0);

        const UVs& uvs = texture.getFrame(frame);
        const Tex3DS_SubTexture subTex = {
            .width  = static_cast<std::uint16_t>(texture.mFrameSize.x),
            .height = static_cast<std::uint16_t>(texture.mFrameSize.y),
            .left   = uvs.left,
            .top    = uvs.top,
            .right  = uvs.right,
            .bottom = uvs.bottom
        };
        image.subtex = &subTex;

        drawImage(image, transform, centre);
    }

    void RenderTarget2D::drawTextureSection(
        const Texture& texture,
        const Transform2D& transform,
        const Rect2& drawSection,
        const bool centre
    ) {
        C2D_Image image = C2D_SpriteSheetGetImage(texture.mRawImage.getNativeHandle(), 0);
        Tex3DS_SubTexture subTex = *image.subtex;
        image.subtex = &subTex;

        const Vector2 pixelUVSize = Vector2(subTex.right - subTex.left, subTex.top - subTex.bottom) / Vector2(texture.mRawImage.getSize());

        subTex.left += pixelUVSize.x * drawSection.position.x;
        subTex.right = subTex.left + pixelUVSize.x * drawSection.size.x;
        subTex.top -= pixelUVSize.y * drawSection.position.y;
        subTex.bottom = subTex.top - pixelUVSize.y * drawSection.size.y;

        subTex.width = static_cast<std::uint16_t>(drawSection.size.x);
        subTex.height = static_cast<std::uint16_t>(drawSection.size.y);

        drawImage(image, transform, centre);
    }

    void RenderTarget2D::drawTexture(
        const Texture& texture,
        const Transform2D& transform,
        const bool centre
    ) {
        drawTextureFrame(texture, transform, 0, centre);
    }

    void RenderTarget2D::drawTextureWithSize(
        const Texture& texture,
        const Transform2D& transform,
        const Vector2& size,
        const std::uint32_t frame,
        const bool centre
    ) {
        C2D_Image image = C2D_SpriteSheetGetImage(texture.mRawImage.getNativeHandle(), 0);

        const UVs uvs = texture.getFrame(frame);
        const Tex3DS_SubTexture subTex = {
            .width  = static_cast<std::uint16_t>(size.x),
            .height = static_cast<std::uint16_t>(size.y),
            .left   = uvs.left,
            .top    = uvs.top,
            .right  = uvs.right,
            .bottom = uvs.bottom
        };
        image.subtex = &subTex;

        drawImage(image, transform, centre);
    }

    void RenderTarget2D::drawCircle(
        const Transform2D& transform,
        const float radius,
        const Colour colour
    ) {
        C2D_DrawCircleSolid(
            std::round(transform.position.x),
            std::round(transform.position.y),
            0,
            radius,
            static_cast<std::uint32_t>(colour)
        );
    }

    void RenderTarget2D::drawRectSolid(
        const Transform2D& transform,
        const Vector2& rectangleSize,
        const Colour colour
    ) {
        const Vector2 screenPos = transform.position;
        C2D_DrawRectSolid(
            std::round(screenPos.x),
            std::round(screenPos.y),
            0,
            rectangleSize.x,
            rectangleSize.y,
            static_cast<std::uint32_t>(colour)
        );
    }

    void RenderTarget2D::drawRect(
        const Transform2D& transform,
        const Vector2& rectangleSize,
        const Colour colour
    ) {
        const Vector2 screenPos = transform.position;
        C2D_DrawRectangle(
            std::round(screenPos.x),
            std::round(screenPos.y),
            0,
            rectangleSize.x,
            rectangleSize.y,
            static_cast<std::uint32_t>(colour),
            static_cast<std::uint32_t>(colour),
            static_cast<std::uint32_t>(colour),
            static_cast<std::uint32_t>(colour)
        );
    }

    void RenderTarget2D::drawRect(
        const Transform2D& transform,
        const Vector2& rectangleSize,
        const std::span<Colour, 4> colours
    ) {
        C2D_DrawRectangle(
            std::round(transform.position.x),
            std::round(transform.position.y),
            0,
            rectangleSize.x,
            rectangleSize.y,
            static_cast<std::uint32_t>(colours[0]),
            static_cast<std::uint32_t>(colours[1]),
            static_cast<std::uint32_t>(colours[2]),
            static_cast<std::uint32_t>(colours[3])
        );
    }

    auto RenderTarget2D::drawChar(
        const Transform2D& transform,
        const Font& font,
        const char c,
        const Colour colour
    ) -> void {
        drawChar(transform, font, font.getGlyph(c), colour);
    }

    void RenderTarget2D::drawChar(
        const Transform2D& transform,
        const Font& font,
        const Font::Glyph& glyph,
        const Colour colour
    ) {
        C2D_Image image = C2D_SpriteSheetGetImage(font.getTexture().mRawImage.getNativeHandle(), 0);
        Tex3DS_SubTexture subTex = *image.subtex;
        image.subtex = &subTex;

        subTex.left = glyph.uvs.left;
        subTex.right = glyph.uvs.right;
        subTex.top = glyph.uvs.top;
        subTex.bottom = glyph.uvs.bottom;

        subTex.width = static_cast<std::uint16_t>(glyph.size.x);
        subTex.height = static_cast<std::uint16_t>(glyph.size.y);

        const Vector2 scaledSize = static_cast<Vector2>(glyph.size) * transform.scale;

        // if (screenPos.x - scaledSize.x >= static_cast<float>(getSize().x)) return;
        // if (screenPos.x + scaledSize.x < 0) return;
        //
        // if (screenPos.y - scaledSize.y >= static_cast<float>(getSize().y)) return;
        // if (screenPos.y + scaledSize.y < 0) return;

        const C2D_DrawParams params = {
            {
                std::round(transform.position.x),
                std::round(transform.position.y),

                scaledSize.x,
                scaledSize.y
            },
            { 0, 0 },
            0,
            transform.rotation
        };

        const C2D_Tint tint = { static_cast<std::uint32_t>(colour), 1.f };
        const C2D_ImageTint imageTint = {tint, tint, tint, tint};
        C2D_DrawImage(image, &params, &imageTint);
    }

    void RenderTarget2D::drawTexture(C3D_Tex* texture, const Transform2D& transform) {
        Tex3DS_SubTexture subTex {
            50,
            50,
            0,
            0,
            1,
            1
        };
        const C2D_Image image = {.tex = texture, .subtex = &subTex};
        C2D_DrawImageAt(image, transform.position.x, transform.position.y, 0);
    }

    void RenderTarget2D::setCameraPos(const Vector2& position) noexcept {
        mCameraPos = position.round();
        C2D_ViewReset();
        const Vector2i halfSize = getSize() / 2;
        C2D_ViewTranslate(-mCameraPos->x + static_cast<float>(halfSize.x), -mCameraPos->y + static_cast<float>(halfSize.y));
    }

    const std::optional<Vector2>& RenderTarget2D::getCameraPos() const noexcept {
        return mCameraPos;
    }

    const std::optional<Rect2>& RenderTarget2D::getScissor() const noexcept {
        return mScissor;
    }

    void RenderTarget2D::clearScissor() noexcept {
        mScissor = {};

        C2D_Flush();
        C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
    }

    void RenderTarget2D::setScissor(const Rect2& rect2) noexcept {
        const int invY = getSize().y - static_cast<int>(rect2.position.y + rect2.size.y);
        const int invX = getSize().x - static_cast<int>(rect2.position.x + rect2.size.x);
        mScissor = rect2;

        const std::uint32_t left = static_cast<std::uint32_t>(std::clamp(invY, 0, getSize().y));
        const std::uint32_t top = static_cast<std::uint32_t>(std::clamp(invX, 0, getSize().x));
        const std::uint32_t right = static_cast<std::uint32_t>(std::clamp(invY + static_cast<int>(rect2.size.y), 0, getSize().y));
        const std::uint32_t bottom = static_cast<std::uint32_t>(std::clamp(invX + static_cast<int>(rect2.size.x), 0, getSize().x));

        C2D_Flush();
        C3D_SetScissor(
            GPU_SCISSOR_NORMAL,
            left,
            top,
            right,
            bottom
        );
    }

    RenderTarget3D::RenderTarget3D(C3D_RenderTarget* target, LightEnv& lightEnv) noexcept
        : mTarget(target)
        , mLightEnv(lightEnv)
    {}

    void RenderTarget3D::clear() noexcept {
        C3D_RenderTargetClear(mTarget, C3D_CLEAR_ALL, 0x6666FFFF, 0);
    }

    void RenderTarget3D::prepare(const float iod) noexcept {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_FrameDrawOn(mTarget);
        program.bind();

        // if (mLightEnv.registerLight(light)) {
        //     light.setColour(1, 1, 1);
        //     light.setPosition(4, 4, 0);
        // } else {
        //     std::cerr << "Failed to register light" << std::endl;
        // }

        // Configure attributes for use with the vertex shader
        C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
        AttrInfo_Init(attrInfo);
        AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);			// v0 = position
        AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);			// v1 = texcoord
        AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3);			// v2 = normal

        AttrInfo_AddLoader(attrInfo, 3, GPU_UNSIGNED_BYTE, 4); // v3 = boneIndices
        AttrInfo_AddLoader(attrInfo, 4, GPU_FLOAT, 4);			// v4 = boneWeights

        mLightEnv.bind();
        mProjection = Matrix4x4::perspStereoTilt(
            deg2rad(40.0f),
            mTarget->screen == GFX_TOP ? C3D_AspectRatioTop : C3D_AspectRatioBot,
            0.01f,
            1000.0f,
            iod,
            3.0f
        );
    }

    void RenderTarget3D::render(const MeshInstance& meshInstance) noexcept {
        const Mesh* mesh = meshInstance.getMesh().get();
 		if (!mesh) return;

 		// Update the uniforms
        {
            C3D_FVec* const ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, uLoc_projection, 4);
            std::memcpy(ptr, &mProjection, sizeof(float) * 4 * 4);
        }
        {
            C3D_FVec* const ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, uLoc_modelView, 3);
            const Matrix4x4 modelView = mCameraInverse * meshInstance.getGlobalTransform();
            std::memcpy(ptr, &modelView, sizeof(float) * 4 * 3);
        }

 		C3D_TexEnv* env = C3D_GetTexEnv(0);
 		C3D_TexEnvInit(env);

        const std::span<const MeshInstance::BoneInstance> boneInstances = meshInstance.getBones();
        const std::span<const Mesh::Bone> bones = mesh->getBones();

 		for (const auto& [material, texture, triangles, boneMappings] : meshInstance.getMesh()->surfaces) {
            if (triangles.empty()) continue;
            // Configure the VBO
            C3D_BufInfo* bufInfo = C3D_GetBufInfo();
            BufInfo_Init(bufInfo);
            BufInfo_Add(bufInfo, triangles.data(), sizeof(Mesh::Vertex), 5, 0x43210);

 			static_assert(std::tuple_size_v<decltype(boneMappings)> < 256);
 			if (!boneInstances.empty()) {
 				for (std::uint8_t i{}; i < static_cast<std::uint8_t>(boneMappings.size()); ++i) {
 					const Matrix4x4& mtx = boneInstances[boneMappings[i]].transform * bones[boneMappings[i]].inverseBindMatrix;
 					C3D_FVec* const ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, uLoc_bones + i * 3, 3);

 					std::memcpy(ptr, &mtx, sizeof(float) * 4 * 3);
 				}
 			}

 			if (texture) {
 				if (mLightEnv.isToonShaded()) {
 					C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_FRAGMENT_SECONDARY_COLOR);
 					C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0);
 					C3D_TexEnvFunc(env, C3D_RGB, GPU_ADD_SIGNED);
 					C3D_TexEnvFunc(env, C3D_Alpha, GPU_ADD);
 				} else {
				    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR);
 					C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
 				}

 				C3D_TexBind(0, C2D_SpriteSheetGetImage(texture->getNativeHandle(), 0).tex);
 			} else {
 				if (mLightEnv.isToonShaded()) {
 					C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
 					C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
 				} else {
 					C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
 					C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
 				}
 			}

 			mLightEnv.material(material);
 			C3D_DrawArrays(GPU_TRIANGLES, 0, static_cast<int>(triangles.size() * 3));
 		}
    }

    void RenderTarget3D::setCameraPos(const Matrix4x4& transform) noexcept {
        mCameraInverse = transform.inverse();
    }

#elifdef M3DS_SFML

    template <typename... Args>
    constexpr auto toCharArray(Args... args) {
        return std::array<char, sizeof...(Args)>{ static_cast<char>(args)... };
    }
    constexpr std::array vShaderRaw = toCharArray(
#embed "../vshader.glsl"
    );

    constexpr std::array fShaderRaw = toCharArray(
#embed "../fshader.glsl"
    );

    constexpr std::string_view vShader { vShaderRaw.data(), vShaderRaw.size() };
    constexpr std::string_view fShader { fShaderRaw.data(), fShaderRaw.size() };


    RenderTarget::RenderTarget(sf::Window window) noexcept
        : mWindow(std::move(window))
    {
        mWindow.setVerticalSyncEnabled(false);

        if (const GLenum code = glewInit(); code != GLEW_OK)
            Debug::terminate("GLEW failed to initialise! ({})", code);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    sf::Window* RenderTarget::get() noexcept {
        return &mWindow;
    }

    Vector2i RenderTarget::getSize() const {
        const auto size = mWindow.getSize();
        return { static_cast<int>(size.x), static_cast<int>(size.y) };
    }

    void RenderTarget::clear([[maybe_unused]] Colour colour, [[maybe_unused]] ClearFlags flags) noexcept {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        glClear(std::to_underlying(flags));
    }

    RenderTarget2D::RenderTarget2D(sf::Window* window) noexcept
        : mWindow(window)
    {}

    void RenderTarget2D::drawTextureFrame(
        [[maybe_unused]] const Texture& texture,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] std::uint32_t frame,
        [[maybe_unused]] bool centre
    ) {
    }

    void RenderTarget2D::drawTexture(
        [[maybe_unused]] const Texture& texture,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] bool centre
    ) {
    }

    void RenderTarget2D::drawTextureWithSize(
        [[maybe_unused]] const Texture& texture,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& size,
        [[maybe_unused]] std::uint32_t frame,
        [[maybe_unused]] bool centre
    ) {
    }

    void RenderTarget2D::drawTextureSection(
        [[maybe_unused]] const Texture& texture,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Rect2& drawSection,
        [[maybe_unused]] bool centre
    ) {
    }

    void RenderTarget2D::drawRectSolid(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& rectangleSize,
        [[maybe_unused]] Colour colour
    ) {
    }

    void RenderTarget2D::drawRect(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& rectangleSize,
        [[maybe_unused]] Colour colour
    ) {
    }

    void RenderTarget2D::drawRect(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& rectangleSize,
        [[maybe_unused]] std::span<Colour, 4> colours
    ) {
    }

    void RenderTarget2D::drawCircle(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] float radius,
        [[maybe_unused]] Colour colour
    ) {
    }

    void RenderTarget2D::drawChar(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Font& font,
        [[maybe_unused]] char c,
        [[maybe_unused]] Colour colour
    ) {
    }

    void RenderTarget2D::drawChar(
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Font& font,
        [[maybe_unused]] const Font::Glyph& glyph,
        [[maybe_unused]] Colour colour
    ) {
    }

    void RenderTarget2D::prepare() noexcept {
    }

    Vector2i RenderTarget2D::getSize() const {
        auto size = mWindow->getSize();

        return { static_cast<int>(size.x), static_cast<int>(size.y) };
    }

    void RenderTarget2D::clear() noexcept {
    }

    void RenderTarget2D::draw(
        [[maybe_unused]] const Style& style,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& boxSize
    ) {
    }

    void RenderTarget2D::draw(
        [[maybe_unused]] const BoxStyle& style,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& boxSize
    ) {
    }

    void RenderTarget2D::draw(
        [[maybe_unused]] const TextureStyle& style,
        [[maybe_unused]] const Transform2D& transform,
        [[maybe_unused]] const Vector2& boxSize
    ) {
    }

    void RenderTarget2D::clearCamera() noexcept {
    }

    void RenderTarget2D::setCameraPos([[maybe_unused]] const Vector2& position) noexcept {
    }

    const std::optional<Vector2>& RenderTarget2D::getCameraPos() const noexcept {
        return mCameraPos;
    }

    void RenderTarget2D::clearScissor() noexcept {
    }

    void RenderTarget2D::setScissor([[maybe_unused]] const Rect2& rect2) noexcept {
    }

    const std::optional<Rect2>& RenderTarget2D::getScissor() const noexcept {
        return mScissor;
    }

    RenderTarget3D::RenderTarget3D(sf::Window* window) noexcept
        : mWindow(window)
    {}

    void RenderTarget3D::clear() noexcept {

    }

    void RenderTarget3D::prepare() noexcept {
        if (!mWindow->setActive(true))
            Debug::terminate("Failed to activate window context!");


    }

    void RenderTarget3D::render([[maybe_unused]] const MeshInstance& meshInstance) noexcept {
    }

    void RenderTarget3D::setCameraPos([[maybe_unused]] const Matrix4x4& transform) noexcept {
    }


#endif
}
