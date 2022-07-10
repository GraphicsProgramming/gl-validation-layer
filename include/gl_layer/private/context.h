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

    void glCompileShader(unsigned int handle);
    void glGetShaderiv(unsigned int handle, GLenum param, int* params);
    void glAttachShader(unsigned int program, unsigned int shader);

    void glGetProgramiv(unsigned int handle, GLenum param, int* params);
    void glLinkProgram(GLuint program);
    void glUseProgram(unsigned int handle);
    void glDeleteProgram(GLuint program);

    void validate_program_bound(std::string_view func_name);
    bool validate_program_status(GLuint program);
    void validate_uniform_location_count(GLint location, GLsizei count);
    void validate_uniform_type(GLint location, std::string_view uniform_func_name);

private:
    Version gl_version;

    GLLayerOutputFun output_fun = nullptr;
    void* output_user_data = nullptr;
    ContextGLFunctions gl;
    GLuint bound_program = 0;

    std::unordered_map<unsigned int, Shader> shaders{};
    std::unordered_map<unsigned int, Program> programs{};

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