#include <gl_layer/context.h>

#include <gl_layer/private/types.h>
#include <gl_layer/private/context.h>

#include <string_view>
#include <cstdio>
#include <cstdarg>

namespace gl_layer {

const char* enum_str(GLenum v) {
    switch(v) {
        case GL_SHADER_TYPE: return "GL_SHADER_TYPE";
        case GL_DELETE_STATUS: return "GL_DELETE_STATUS";
        case GL_COMPILE_STATUS: return "GL_COMPILE_STATUS";
        case GL_LINK_STATUS: return "GL_LINK_STATUS";
        case GL_INFO_LOG_LENGTH: return "GL_INFO_LOG_LENGTH";
        case GL_SHADER_SOURCE_LENGTH: return "GL_SHADER_SOURCE_LENGTH";
        default:
            return "";
    }
}

static void default_output_func(const char* text, void* = nullptr) {
    printf("%s\n", text);
}

Context::Context(Version version, const ContextGLFunctions* gl_functions) 
  : gl_version(version), gl(*gl_functions) {
    output_fun = &default_output_func;
}

void Context::set_output_callback(GLLayerOutputFun callback, void* user_data) {
    output_fun = callback;
    output_user_data = user_data;
}


namespace {
Context* g_context = nullptr;
}

} // namespace gl_layer

static bool is_func(std::string_view name, std::string_view func) {
    return func == name;
}

static bool func_has(std::string_view name, std::string_view substr) {
    return name.find(substr) != std::string_view::npos;
}

int gl_layer_init(unsigned int gl_version_major, unsigned int gl_version_minor, const ContextGLFunctions* gl_functions) {
    gl_layer::g_context = new gl_layer::Context(gl_layer::Version{ gl_version_major, gl_version_minor }, gl_functions);
    if (gl_layer::g_context) return 0;

    return -1;
}

void gl_layer_terminate() {
    delete gl_layer::g_context;
}

void gl_layer_callback(const char* name_c, void* func_ptr, int num_args, ...) {
    if (!gl_layer::g_context) {
        // Report error: context not initialized.
        return;
    }

    using namespace gl_layer; // GL types

    std::string_view name = name_c;

    va_list args;
    va_start(args, num_args);

    if (is_func(name, "glCompileShader")) {
        auto shader = va_arg(args, GLuint);
        gl_layer::g_context->glCompileShader(shader);
    } else if (is_func(name, "glGetShaderiv")) {
        auto shader = va_arg(args, GLuint);
        gl_layer::GLenum param = va_arg(args, GLenum);
        auto* params = va_arg(args, GLint*);
        gl_layer::g_context->glGetShaderiv(shader, param, params);
    } else if (is_func(name, "glAttachShader")) {
        // Note that the parameters must be pulled outside the function call, since there is no guarantee that they will evaluate in order!
        auto program = va_arg(args, GLuint);
        auto shader = va_arg(args, GLuint);
        gl_layer::g_context->glAttachShader(program, shader);
    } else if (is_func(name, "glGetProgramiv")) {
        auto program = va_arg(args, GLuint);
        gl_layer::GLenum param = va_arg(args, GLenum);
        auto* params = va_arg(args, GLint*);
        gl_layer::g_context->glGetProgramiv(program, param, params);
    } else if (is_func(name, "glUseProgram")) {
        auto program = va_arg(args, GLuint);
        gl_layer::g_context->glUseProgram(program);
    } else if (is_func(name, "glLinkProgram")) {
        auto program = va_arg(args, GLuint);
        gl_layer::g_context->glLinkProgram(program);
    }

    va_end(args);
}

[[maybe_unused]] void gl_layer_set_output_callback(GLLayerOutputFun callback, void* user_data) {
    if (!gl_layer::g_context) {
        // Report error: context not initialized.
        return;
    }
    gl_layer::g_context->set_output_callback(callback, user_data);
}