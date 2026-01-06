#pragma once

#include <cstdint>
#include <cstring>

#include "m3ds/spatial/Matrix4x4.hpp"

#ifdef __3DS__
#include <m3ds/render/DVLB.hpp>
#elifdef M3DS_SFML
#include <SFML/Graphics/Shader.hpp>
#endif


namespace M3DS {
#ifdef __3DS__
    class ShaderProgram {
        DVLB dvlb {};
        shaderProgram_s program {};
    public:
        constexpr ShaderProgram(const ShaderProgram&) noexcept = delete;
        constexpr ShaderProgram& operator=(const ShaderProgram&) noexcept = delete;

        constexpr ShaderProgram(ShaderProgram&& other) noexcept;
        constexpr ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        template <typename T, std::size_t len>
        explicit ShaderProgram(std::span<T, len> shader) noexcept;

        void bind() noexcept;

        std::int8_t getUniformLocation(const char* name) const noexcept;
        void updateUniform(std::int8_t location, const Matrix4x4& value) noexcept;
    };

    template <typename T, std::size_t len>
    ShaderProgram::ShaderProgram(std::span<T, len> shader) noexcept: dvlb(shader) {
        shaderProgramInit(&program);
        shaderProgramSetVsh(&program, dvlb->DVLE);
    }

    constexpr ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
        : dvlb(std::exchange(other.dvlb, {}))
        , program(std::exchange(other.program, {}))
    {}

    constexpr ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
        if (this != &other) {
            dvlb = std::exchange(other.dvlb, {});
            program = std::exchange(other.program, {});
        }
        return *this;
    }

    inline void ShaderProgram::bind() noexcept {
        C3D_BindProgram(&program);
    }

    inline std::int8_t ShaderProgram::getUniformLocation(const char* name) const noexcept {
        return shaderInstanceGetUniformLocation(program.vertexShader, name);
    }

    inline void ShaderProgram::updateUniform(std::int8_t location, const Matrix4x4& value) noexcept {
        C3D_FVec* ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, location, 4);
        std::memcpy(ptr, &value, sizeof(value));
    }
#elifdef M3DS_SFML
    using ShaderProgram = sf::Shader;
#endif
}
