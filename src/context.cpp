#include <gl_layer/context.h>

#include <string_view>
#include <cstdio>
#include <cstdarg>
#include <cassert>

#include <unordered_map>

namespace gl_layer {

using GLenum = unsigned int;

enum GLShaderInfoParam {
    GL_SHADER_TYPE = 0x8B4F,
    GL_DELETE_STATUS = 0x8B80,
    GL_COMPILE_STATUS = 0x8B81,
    GL_INFO_LOG_LENGTH = 0x8B84,
    GL_SHADER_SOURCE_LENGTH = 0x8B88
};

static const char* enum_str(GLenum v) {
    switch(v) {
        case GL_SHADER_TYPE: return "GL_SHADER_TYPE";
        case GL_DELETE_STATUS: return "GL_DELETE_STATUS";
        case GL_COMPILE_STATUS: return "GL_COMPILE_STATUS";
        case GL_INFO_LOG_LENGTH: return "GL_INFO_LOG_LENGTH";
        case GL_SHADER_SOURCE_LENGTH: return "GL_SHADER_SOURCE_LENGTH";
        default:
            return "";
    }
}

static void default_output_func(const char* text, void* = nullptr) {
    printf("%s", text);
}

struct Version {
    unsigned int major = 0;
    unsigned int minor = 0;
};

// Represents a shader returned by glCreateShader
struct Shader {
    unsigned int handle {};
    // If this is -1, this means the compile status was never checked by the host application.
    // This is an error that should be reported.
    int compile_status = -1;
};

class Context {
public:
    explicit Context(Version version) : gl_version(version) {

    }

    void set_output_callback(GLLayerOutputFun callback, void* user_data) {
        output_fun = callback;
        output_user_data = user_data;
    }

    void glCompileShader(unsigned int handle) {
        // We will use glCompileShader to add shaders to our internal structure, since
        // we cannot access the return value from glCreateShader()

        auto it = shaders.find(handle);
        if (it != shaders.end()) {
            output_fmt("glCompileShader(shader = %u): Shader is already compiled.", handle);
            return;
        }

        shaders.insert({ handle, Shader { handle } });
    }

    void glGetShaderiv(unsigned int handle, GLenum param, int* params) {
        assert(params && "params may not be nullptr");

        if (param == GL_COMPILE_STATUS) {
            auto it = shaders.find(handle);
            if (it == shaders.end()) {
                output_fmt("glGetShaderiv(handle = %u, param = %s, params = %p): Invalid shader handle.", handle, enum_str(param), static_cast<void*>(params));
                return;
            }

            it->second.compile_status = *params;
        }
    }

    void glAttachShader(unsigned int program, unsigned int shader) {
        // Make sure compile status was checked and successful when attaching a shader.
        auto it = shaders.find(shader);
        if (it == shaders.end()) {
            output_fmt("glAttachShader(program = %u, shader = %u): Invalid shader handle.", program, shader);
            return;
        }

        if (it->second.compile_status == -1) {
            output_fmt("glAttachShader(program = %u, shader = %u): Always check shader compilation status before trying to use the object.", program, shader);
        } else if (it->second.compile_status == false) {
            output_fmt("glAttachShader(program = %u, shader = %u): Attached shader has a compilation error.", program, shader);
        }
    }

private:
    Version gl_version;

    GLLayerOutputFun output_fun = &default_output_func;
    void* output_user_data = nullptr;

    // Note: Not programs!
    std::unordered_map<unsigned int, Shader> shaders {};

    template<typename... Args>
    void output_fmt(const char* fmt, Args&&... args) {
        std::size_t size = static_cast<std::size_t>(std::snprintf(nullptr, 0, fmt, args...)) + 1; // Extra space for null terminator
        assert(size > 0 && "Error during formatting");
        char* buf = new char[size];
        std::snprintf(buf, size, fmt, args...);
        output_fun(buf, output_user_data);
        delete[] buf;
    }
};


namespace {
Context* g_context = nullptr;
}

} // namespace gl_layer

static bool is_func(const char* name, std::string_view func) {
    return func == name;
}

int gl_layer_init(unsigned int gl_version_major, unsigned int gl_version_minor) {
    gl_layer::g_context = new gl_layer::Context(gl_layer::Version{ gl_version_major, gl_version_minor });
    if (gl_layer::g_context) return 0;

    return -1;
}

void gl_layer_terminate() {
    delete gl_layer::g_context;
}

void gl_layer_callback(const char* name, void* func_ptr, int num_args, ...) {
    if (!gl_layer::g_context) {
        // Report error: context not initialized.
        return;
    }

    va_list args;
    va_start(args, num_args);

    if (is_func(name, "glCompileShader")) {
        gl_layer::g_context->glCompileShader(va_arg(args, unsigned int));
    } else if (is_func(name, "glGetShaderiv")) {
        unsigned int shader = va_arg(args, unsigned int);
        gl_layer::GLenum param = va_arg(args, gl_layer::GLenum);
        int* params = va_arg(args, int*);
        gl_layer::g_context->glGetShaderiv(shader, param, params);
    } else if (is_func(name, "glAttachShader")) {
        // Note that the parameters must be pulled outside the function call, since there is no guarantee that they will evaluate in order!
        unsigned int program = va_arg(args, unsigned int);
        unsigned int shader = va_arg(args, unsigned int);
        gl_layer::g_context->glAttachShader(program, shader);
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