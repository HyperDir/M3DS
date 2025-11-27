#pragma once

#include <cstdint>

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

        constexpr ShaderProgram(ShaderProgram&& other) noexcept
            : dvlb(std::exchange(other.dvlb, {}))
            , program(std::exchange(other.program, {}))
        {}
        constexpr ShaderProgram& operator=(ShaderProgram&& other) noexcept {
            if (this != &other) {
                dvlb = std::exchange(other.dvlb, {});
                program = std::exchange(other.program, {});
            }
            return *this;
        }

        template <typename T, std::size_t len>
        explicit ShaderProgram(std::span<T, len> shader) noexcept
            : dvlb(shader)
        {
            shaderProgramInit(&program);
            shaderProgramSetVsh(&program, dvlb->DVLE);
        }

        void bind() noexcept {
            C3D_BindProgram(&program);
        }

        std::int8_t getUniformLocation(const char* name) const noexcept {
            return shaderInstanceGetUniformLocation(program.vertexShader, name);
        }
    };
#elifdef M3DS_SFML
    using ShaderProgram = sf::Shader;
#endif
}
