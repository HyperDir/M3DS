#include <m3ds/render/ShaderProgram.hpp>

namespace M3DS {
    void ShaderProgram::bind() noexcept {
        C3D_BindProgram(&program);
    }

    std::int8_t ShaderProgram::getUniformLocation(const char* name) const noexcept {
        return shaderInstanceGetUniformLocation(program.vertexShader, name);
    }

    void ShaderProgram::updateUniform4x4(const std::int8_t location, const Matrix4x4& value) noexcept {
        C3D_FVec* ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, location, 4);
        std::memcpy(ptr, &value, sizeof(value));
    }

    void ShaderProgram::updateUniform4x3(std::int8_t location, const Matrix4x4& value) noexcept {
        C3D_FVec* ptr = C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, location, 3);
        std::memcpy(ptr, &value, sizeof(value) * 3 / 4);
    }
}
