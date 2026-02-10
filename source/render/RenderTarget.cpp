#include <cassert>
#include <m3ds/render/RenderTarget.hpp>

#include <cmath>

#include <m3ds/nodes/3d/MeshInstance.hpp>

#include <cstring>

namespace M3DS {
    constinit std::array shader3d = std::to_array<unsigned char>({
#embed <3dshader.bin>
    });

    constinit std::array shader2d = std::to_array<unsigned char>({
#embed <2dshader.bin>
    });
}

namespace M3DS {
    ShaderProgram RenderTarget3D::program { std::span{ shader3d } };
    
    std::int8_t RenderTarget3D::uLoc_projection = program.getUniformLocation("projection");
    std::int8_t RenderTarget3D::uLoc_modelView = program.getUniformLocation("modelView");
    std::int8_t RenderTarget3D::uLoc_bones = program.getUniformLocation("bones");


    ShaderProgram RenderTarget2D::program { std::span{ shader2d } };

    std::int8_t RenderTarget2D::uLoc_projection = program.getUniformLocation("projection");
    std::int8_t RenderTarget2D::uLoc_modelView = program.getUniformLocation("modelView");
}

namespace M3DS {
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
            GSP_SCREEN_WIDTH,
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
                GSP_SCREEN_WIDTH,
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

    RenderTarget2D::~RenderTarget2D() noexcept {
        disablePrimitive();
        mScratchBuffer.swap();
    }

    Vector2i RenderTarget2D::getSize() const {
        return { mTarget->screen == GFX_TOP ? 400 : 320, 240 };
    }

    void RenderTarget2D::enablePrimitive() noexcept {
        if (mPrimitive)
            return;

        program.updateUniform(uLoc_modelView, mCameraInverse);
        setTint(Colours::white);
        mPrimitive = true;
    }

    void RenderTarget2D::disablePrimitive() noexcept {
        if (!mPrimitive)
            return;

        primitiveFlush();

        mPrimitive = false;
        setTint(Colours::white);
    }

    void RenderTarget2D::primitiveSendVertex(const Vertex2D& vertex) noexcept {
        if (Failure failure = mScratchBuffer.emplace(vertex))
            Debug::err(failure);
    }

    void RenderTarget2D::primitiveFlush() noexcept {
        assert(mPrimitive && "Must be in primitive mode to flush!");

        if (const std::span buf = mScratchBuffer.getCurrentSpan(); !buf.empty()) {
            C3D_BufInfo* bufInfo = C3D_GetBufInfo();
            BufInfo_Init(bufInfo);
            BufInfo_Add(bufInfo, buf.data(), sizeof(Vertex2D), 3, 0x210);

            C3D_DrawArrays(GPU_TRIANGLES, 0, static_cast<int>(buf.size()));
            mScratchBuffer.startNewSpan();
        }
    }

    void RenderTarget2D::bind(C3D_Tex* texture) noexcept {
        if (mBoundTexture == texture)
            return;

        if (mPrimitive)
            primitiveFlush();

        mBoundTexture = texture;

        if (texture) {
 		    C3D_TexEnv* env = C3D_GetTexEnv(0);
		    C3D_TexEnvInit(env);
            C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
            C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0);

            C3D_TexBind(0, texture);
        } else {
            C3D_TexEnvSrc(C3D_GetTexEnv(0), C3D_Both, GPU_PRIMARY_COLOR);
        }
    }

    void RenderTarget2D::setTint(const Colour colour) noexcept {
        if (mTintColour == colour)
            return;

        if (mPrimitive)
            primitiveFlush();

        mTintColour = colour;
        C3D_TexEnvColor(C3D_GetTexEnv(1), mTintColour.convertForGPU());
    }

    void RenderTarget2D::draw(const Triangle2D& triangle, C3D_Tex* texture) noexcept {
        bind(texture);

        enablePrimitive();
        for (const auto& v : triangle)
            primitiveSendVertex(v);
    }

    void RenderTarget2D::draw(const Quad2D& quad, C3D_Tex* texture) noexcept {
        bind(texture);

        enablePrimitive();
        primitiveSendVertex(quad[0]);
        primitiveSendVertex(quad[1]);
        primitiveSendVertex(quad[2]);
        primitiveSendVertex(quad[1]);
        primitiveSendVertex(quad[2]);
        primitiveSendVertex(quad[3]);
    }

    void RenderTarget2D::draw(const Mesh2D& mesh) noexcept {
        disablePrimitive();

        const Matrix4x4 modelView = mCameraInverse * mesh.transform;

        program.updateUniform(uLoc_modelView, modelView);

        C3D_BufInfo* bufInfo = C3D_GetBufInfo();
        BufInfo_Init(bufInfo);
        BufInfo_Add(bufInfo, mesh.vertices.data(), sizeof(Vertex2D), 3, 0x210);

        bind(mesh.texture.getNative());
        setTint(mesh.tint);

        C3D_DrawArrays(GPU_TRIANGLES, 0, static_cast<int>(mesh.vertices.size()));
    }

    void RenderTarget2D::drawTextureFrame(
        const SpriteSheet& spriteSheet,
        const Transform2D& transform,
        const uint32_t frame,
        const bool centre
    ) noexcept {
        const float sinTheta = std::sin(transform.rotation);
        const float cosTheta = std::cos(transform.rotation);

        const Pixels<Vector2>& position = transform.position;
        const Vector2& scale = transform.scale;
        const Pixels<Vector2>& size = spriteSheet.getFrameSize() * scale;
        const UVs& uvs = spriteSheet.getFrame(frame);

        Vector2 min {};
        Vector2 max {};

        if (centre) {
            const Vector2 halfSize = size * 0.5f;
            min = -halfSize;
            max = halfSize;
        } else {
            min = {};
            max = size;
        }

        auto transformVertex = [&](const float localX, const float localY) {
            return Vector2{
                localX * cosTheta - localY * sinTheta + position.x,
                localX * sinTheta + localY * cosTheta + position.y
            };
        };

        const Vector2 topLeft = transformVertex(min.x, min.y);
        const Vector2 bottomLeft = transformVertex(min.x, max.y);
        const Vector2 topRight = transformVertex(max.x, min.y);
        const Vector2 bottomRight = transformVertex(max.x, max.y);

        const Quad2D quad {{
            { { topLeft.x, topLeft.y, 0.f }, { uvs.left,  uvs.top } },
            { { bottomLeft.x, bottomLeft.y, 0.f }, { uvs.left,  uvs.bottom } },
            { { topRight.x, topRight.y, 0.f }, { uvs.right, uvs.top } },
            { { bottomRight.x, bottomRight.y, 0.f }, { uvs.right, uvs.bottom } }
        }};

        draw(quad, spriteSheet.getNative());
    }

    void RenderTarget2D::draw(const Style& style, const Transform2D& transform, const Vector2& boxSize) {
        style.visit([&](const StyleInterface auto& s) {
            draw(s, transform, boxSize);
        });
    }

    void RenderTarget2D::draw(const BoxStyle& style, const Transform2D& transform, const Vector2& boxSize) {
        bind(style.texture.getNative());
        enablePrimitive();

        const float sinTheta = std::sin(transform.rotation);
        const float cosTheta = std::cos(transform.rotation);

        const Pixels<Vector2>& position = transform.position;
        const Vector2& scale = transform.scale;
        const Pixels<Vector2>& size = boxSize * scale;

        const auto transformVertex = [&](const float localX, const float localY) {
            return Vector2{
                localX * cosTheta - localY * sinTheta + position.x,
                localX * sinTheta + localY * cosTheta + position.y
            };
        };

        const Vector2 topLeft = transformVertex(0, 0);
        const Vector2 bottomLeft = transformVertex(0, size.y);
        const Vector2 topRight = transformVertex(size.x, 0);
        const Vector2 bottomRight = transformVertex(size.x, size.y);

        const Vector2 horizUnit = Vector2{style.texture.getSize().x * scale.x, 0.f }.rotated(sinTheta, cosTheta);
        const Vector2 vertUnit = Vector2{0.f, style.texture.getSize().y * scale.y }.rotated(sinTheta, cosTheta);

        const auto toVec3 = [](Vector2 vec) -> Vector3 {
            return { vec.x, vec.y, 0.f };
        };

        const UVs& uvs = style.texture.getUvs();

        // Multiply by 0.9999f to avoid black areas on edge in non-power of 2 textures
        const std::array<Vector2, 4> texcoords {{
            { uvs.left, uvs.top },
            { uvs.right * 0.9999f, uvs.top },
            { uvs.left, uvs.bottom },
            { uvs.right * 0.9999f, uvs.bottom }
        }};

        const std::array<Vertex2D, 16> vertices {{
            { toVec3(topLeft), texcoords[0], style.colour },
            { toVec3(topLeft + horizUnit), texcoords[1], style.colour},
            {toVec3(topLeft + vertUnit), texcoords[2], style.colour },
            { toVec3(topLeft + vertUnit + horizUnit), texcoords[3], style.colour },
            { toVec3(topRight), texcoords[0], style.colour },
            { toVec3(topRight - horizUnit), texcoords[1], style.colour },
            { toVec3(topRight + vertUnit), texcoords[2], style.colour },
            { toVec3(topRight + vertUnit - horizUnit), texcoords[3], style.colour },

            { toVec3(bottomLeft), texcoords[0], style.colour },
            { toVec3(bottomLeft + horizUnit), texcoords[1], style.colour},
            {toVec3(bottomLeft - vertUnit), texcoords[2], style.colour },
            { toVec3(bottomLeft - vertUnit + horizUnit), texcoords[3], style.colour },
            { toVec3(bottomRight), texcoords[0], style.colour },
            { toVec3(bottomRight - horizUnit), texcoords[1], style.colour },
            { toVec3(bottomRight - vertUnit), texcoords[2], style.colour },
            { toVec3(bottomRight - vertUnit - horizUnit), texcoords[3], style.colour },
        }};

        // Top Left Corner
        primitiveSendVertex(vertices[0]);
        primitiveSendVertex(vertices[1]);
        primitiveSendVertex(vertices[2]);
        primitiveSendVertex(vertices[2]);
        primitiveSendVertex(vertices[1]);
        primitiveSendVertex(vertices[3]);

        // Top Right Corner
        primitiveSendVertex(vertices[4]);
        primitiveSendVertex(vertices[5]);
        primitiveSendVertex(vertices[6]);
        primitiveSendVertex(vertices[6]);
        primitiveSendVertex(vertices[5]);
        primitiveSendVertex(vertices[7]);

        // Bottom Left Corner
        primitiveSendVertex(vertices[8]);
        primitiveSendVertex(vertices[9]);
        primitiveSendVertex(vertices[10]);
        primitiveSendVertex(vertices[10]);
        primitiveSendVertex(vertices[9]);
        primitiveSendVertex(vertices[11]);

        // Bottom Right Corner
        primitiveSendVertex(vertices[12]);
        primitiveSendVertex(vertices[13]);
        primitiveSendVertex(vertices[14]);
        primitiveSendVertex(vertices[14]);
        primitiveSendVertex(vertices[13]);
        primitiveSendVertex(vertices[15]);

        // Top Bar
        primitiveSendVertex(vertices[1]);
        primitiveSendVertex(vertices[5]);
        primitiveSendVertex(vertices[3]);
        primitiveSendVertex(vertices[3]);
        primitiveSendVertex(vertices[5]);
        primitiveSendVertex(vertices[7]);

        // Bottom Bar
        primitiveSendVertex(vertices[9]);
        primitiveSendVertex(vertices[13]);
        primitiveSendVertex(vertices[11]);
        primitiveSendVertex(vertices[11]);
        primitiveSendVertex(vertices[13]);
        primitiveSendVertex(vertices[15]);

        // Left Bar
        primitiveSendVertex(vertices[2]);
        primitiveSendVertex(vertices[3]);
        primitiveSendVertex(vertices[10]);
        primitiveSendVertex(vertices[10]);
        primitiveSendVertex(vertices[3]);
        primitiveSendVertex(vertices[11]);

        // Right Bar
        primitiveSendVertex(vertices[6]);
        primitiveSendVertex(vertices[7]);
        primitiveSendVertex(vertices[14]);
        primitiveSendVertex(vertices[14]);
        primitiveSendVertex(vertices[7]);
        primitiveSendVertex(vertices[15]);

        // Centre
        primitiveSendVertex(vertices[3]);
        primitiveSendVertex(vertices[7]);
        primitiveSendVertex(vertices[11]);
        primitiveSendVertex(vertices[11]);
        primitiveSendVertex(vertices[7]);
        primitiveSendVertex(vertices[15]);
    }

    void RenderTarget2D::draw(const TextureStyle& style, const Transform2D& transform, const Vector2& boxSize) {
        bind(style.texture.getNative());
        enablePrimitive();

        const float sinTheta = std::sin(transform.rotation);
        const float cosTheta = std::cos(transform.rotation);

        const Pixels<Vector2>& position = transform.position;
        const Vector2& scale = transform.scale;
        const Pixels<Vector2>& size = boxSize * scale;

        const auto transformVertex = [&](const float localX, const float localY) {
            return Vector2{
                localX * cosTheta - localY * sinTheta + position.x,
                localX * sinTheta + localY * cosTheta + position.y
            };
        };

        const Vector2 topLeft = transformVertex(0, 0);
        [[maybe_unused]] const Vector2 bottomLeft = transformVertex(0, size.y);
        [[maybe_unused]] const Vector2 topRight = transformVertex(size.x, 0);
        [[maybe_unused]] const Vector2 bottomRight = transformVertex(size.x, size.y);

        const UVs& uvs = style.texture.getFrame(style.frame);

        const auto toVec3 = [](Vector2 vec) -> Vector3 {
            return { vec.x, vec.y, 0.f };
        };

        const std::array<Vertex2D, 4> vertices {{
            { toVec3(topLeft), { uvs.left, uvs.top } },
            { toVec3(topRight), { uvs.right, uvs.top } },
            { toVec3(bottomLeft), { uvs.left, uvs.bottom } },
            { toVec3(bottomRight), { uvs.right, uvs.bottom } },
        }};

        primitiveSendVertex(vertices[0]);
        primitiveSendVertex(vertices[1]);
        primitiveSendVertex(vertices[2]);
        primitiveSendVertex(vertices[2]);
        primitiveSendVertex(vertices[1]);
        primitiveSendVertex(vertices[3]);
    }

    void RenderTarget2D::drawRectSolid(const Transform2D& transform, Vector2 size, const Colour colour) {
        const float sinTheta = std::sin(transform.rotation);
        const float cosTheta = std::cos(transform.rotation);

        size *= transform.scale;

        const auto transformVertex = [&](const float localX, const float localY) {
            return Vector2{
                localX * cosTheta - localY * sinTheta + transform.position.x,
                localX * sinTheta + localY * cosTheta + transform.position.y
            };
        };

        const Vector2 topLeft = transformVertex(0, 0);
        const Vector2 bottomLeft = transformVertex(0, size.y);
        const Vector2 topRight = transformVertex(size.x, 0);
        const Vector2 bottomRight = transformVertex(size.x, size.y);

        const Quad2D quad {{
            { { topLeft.x, topLeft.y, 0.f }, {}, colour },
            { { bottomLeft.x, bottomLeft.y, 0.f }, {}, colour },
            { { topRight.x, topRight.y, 0.f }, {}, colour },
            { { bottomRight.x, bottomRight.y, 0.f }, {}, colour }
        }};

        draw(quad);
    }

    void RenderTarget2D::prepare() noexcept {
        C3D_FrameDrawOn(mTarget);
        program.bind();

        // Configure attributes for use with the vertex shader
        C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
        AttrInfo_Init(attrInfo);
        AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); 		// v0=position
        AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); 		// v1=texcoord
        AttrInfo_AddLoader(attrInfo, 2, GPU_UNSIGNED_BYTE, 4); // v2=color

        C3D_TexEnv* env0 = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env0);
        C3D_TexEnvSrc(env0, C3D_Both, GPU_TEXTURE0);
        C3D_TexEnvFunc(env0, C3D_Both, GPU_MODULATE);

        C3D_TexEnv* env1 = C3D_GetTexEnv(1);
        C3D_TexEnvInit(env1);
        C3D_TexEnvSrc(env1, C3D_Both, GPU_PREVIOUS, GPU_CONSTANT);
        C3D_TexEnvFunc(env1, C3D_Both, GPU_MODULATE);
        C3D_TexEnvColor(env1, mTintColour.convertForGPU());

        C3D_TexEnv* env2 = C3D_GetTexEnv(2);
        C3D_TexEnvInit(env2);
        C3D_TexEnvSrc(env2, C3D_Both, GPU_PREVIOUS, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env2, C3D_Both, GPU_MODULATE);


        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

        C3D_TexBind(0, nullptr);
        mBoundTexture = nullptr;

        C3D_CullFace(GPU_CULL_NONE);

		program.updateUniform(uLoc_projection, mProjection);
    }

    void RenderTarget2D::clear() noexcept {
        C3D_RenderTargetClear(mTarget, C3D_CLEAR_ALL, Colours::black.convertForFrameBuf(), 0);
    }

    void RenderTarget2D::clearCamera() noexcept {
        mCameraPos = {};
        mCameraInverse = Matrix4x4::identity();
    }

    void RenderTarget2D::setCameraPos(const Vector2& position) noexcept {
        mCameraPos = position.round();

        const Vector2 halfSize = static_cast<Vector2>(getSize() / 2);

        mCameraInverse = Matrix4x4::fromTranslation({ mCameraPos->x - halfSize.x, mCameraPos->y - halfSize.y, 0.f }).inverse();
    }

    const std::optional<Vector2>& RenderTarget2D::getCameraPos() const noexcept {
        return mCameraPos;
    }

    const std::optional<Rect2>& RenderTarget2D::getScissor() const noexcept {
        return mScissor;
    }

    void RenderTarget2D::clearScissor() noexcept {
        mScissor = {};

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
        program.bind();

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

        C3D_CullFace(GPU_CULL_BACK_CCW);
    }

    void RenderTarget3D::render(const MeshInstance& meshInstance) noexcept {
        const Mesh* mesh = meshInstance.getMesh().get();
 		if (!mesh) return;

 		// Update the uniforms
        program.updateUniform(uLoc_projection, mProjection);
        program.updateUniform(uLoc_modelView, mCameraInverse * meshInstance.getGlobalTransform());

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

 				C3D_TexBind(0, texture->getNative());
 			} else {
 				if (mLightEnv.isToonShaded()) {
 					C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
 					C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
 				} else {
 					C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
 					C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
 				}
 			}

 		    C3D_TexEnvInit(C3D_GetTexEnv(1));
            C3D_TexEnvInit(C3D_GetTexEnv(2));

 			mLightEnv.material(material);
 			C3D_DrawArrays(GPU_TRIANGLES, 0, static_cast<int>(triangles.size() * 3));
 		}
    }

    void RenderTarget3D::setCameraPos(const Matrix4x4& transform) noexcept {
        mCameraInverse = transform.inverse();
    }
}
