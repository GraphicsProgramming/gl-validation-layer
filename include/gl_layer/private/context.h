#ifndef GL_VALIDATION_LAYER_PRIVATE_CONTEXT_H_
#define GL_VALIDATION_LAYER_PRIVATE_CONTEXT_H_

#include <gl_layer/context.h>
#include <gl_layer/private/types.h>

#include <unordered_map>
#include <cassert>

namespace gl_layer {

class Context {
public:
    explicit Context(Version version, const ContextGLFunctions* gl_functions);

    void set_output_callback(GLLayerOutputFun callback, void* user_data);

    void glCompileShader(GLuint program);
    void glGetShaderiv(GLuint program, GLenum param, GLint* params);
    void glAttachShader(GLuint program, GLuint shader);

    void glGetProgramiv(GLuint program, GLenum param, GLint* params);
    void glLinkProgram(GLuint program);
    void glUseProgram(GLuint program);
    void glDeleteProgram(GLuint program);

    void glGenTextures(GLsizei count, GLuint* handles);
    void glCreateTextures(GLenum target, GLsizei count, GLuint* handles);
    void glBindTexture(GLenum target, GLuint texture);
    void glDeleteTextures(GLsizei count, GLuint* handles);

    void validate_program_bound(std::string_view func_name);
    bool validate_program_status(GLuint program);

private:
    Version gl_version;

    GLLayerOutputFun output_fun = nullptr;
    void* output_user_data = nullptr;
    ContextGLFunctions gl;
    GLuint current_program_handle = 0;

    // One entry per texture target
    std::unordered_map<GLTextureTarget, GLuint> bound_textures{};

    std::unordered_map<GLuint, Shader> shaders{};
    std::unordered_map<GLuint, Program> programs{};
    std::unordered_map<GLuint, Texture> textures{};

    template<typename... Args>
    void output_fmt(const char* fmt, Args&& ... args) {
        std::size_t size = static_cast<std::size_t>(std::snprintf(nullptr, 0, fmt, args...)) + 1; // Extra space for null terminator
        assert(size > 0 && "Error during formatting");
        char* buf = new char[size];
        std::snprintf(buf, size, fmt, args...);
        output_fun(buf, output_user_data);
        delete[] buf;
    }
};

}

#endif