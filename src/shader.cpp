#include <gl_layer/context.h>
#include <gl_layer/private/context.h>


namespace gl_layer {

void Context::glCompileShader(unsigned int handle) {
    // We will use glCompileShader to add shaders to our internal structure, since
    // we cannot access the return value from glCreateShader()

    auto it = shaders.find(handle);
    if (it != shaders.end()) {
        output_fmt("glCompileShader(shader = %u): Shader is already compiled.", handle);
        return;
    }

    shaders.insert({ handle, Shader { handle } });
}

void Context::glGetShaderiv(unsigned int handle, GLenum param, int* params) {
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

void Context::glAttachShader(unsigned int program, unsigned int shader) {
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

    // We will also use glAttachShader to create and manage program variables, as we cannot use glCreateProgram for this.
    auto program_it = programs.find(program);
    if (program_it == programs.end()) {
        // This is the first time this program gets a shader attached, create a new one.
        Program prog = { program, { shader } };
        programs.insert({ program, std::move(prog) });
    } else {
        // Program already has an attached shader, add this one as well
        program_it->second.shaders.push_back(shader);
    }
}

void Context::glGetProgramiv(unsigned int handle, GLenum param, int* params) {
    assert(params && "params may not be nullptr");

    if (param == GL_LINK_STATUS) {
        auto it = programs.find(handle);
        if (it == programs.end()) {
            output_fmt("glGetProgramiv(handle = %u, param = %s, params = %p): Invalid program handle.", handle, enum_str(param), static_cast<void*>(params));
            return;
        }

        it->second.link_status = *params;
    }
}

void Context::glUseProgram(unsigned int handle) {
    auto it = programs.find(handle);
    if (it == programs.end()) {
        output_fmt("glUseProgram(program = %u): Invalid program handle.", handle);
        return;
    }

    if (it->second.link_status == -1) {
        output_fmt("glUseProgram(program = %u): Always check program link status before trying to use the object.", handle);
        return;
    }

    if (it->second.link_status == false) {
        output_fmt("glUseProgram(program = %u): Program has a linker error.", handle);
        return;
    }
}

}