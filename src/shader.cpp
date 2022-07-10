#include <gl_layer/context.h>
#include <gl_layer/private/context.h>
#include <memory>

namespace gl_layer {

void Context::glCompileShader(unsigned int handle) {
    // We will use glCompileShader to add shaders to our internal structure, since
    // we cannot access the return value from glCreateShader()

    auto it = shaders.find(handle);
    if (it != shaders.end()) {
        output_fmt("glCompileShader(shader = %u): Shader is already compiled.", handle);
        return;
    }

    shaders.emplace(handle, Shader{ handle });
}

void Context::glGetShaderiv(unsigned int handle, GLenum param, int* params) {
    assert(params && "params may not be nullptr");

    if (param == GL_COMPILE_STATUS) {
        auto it = shaders.find(handle);
        if (it == shaders.end()) {
            output_fmt("glGetShaderiv(handle = %u, param = %s, params = %p): Invalid shader handle.", handle, enum_str(param), static_cast<void*>(params));
            return;
        }

        it->second.compile_status = static_cast<CompileStatus>(*params);
    }
}

void Context::glAttachShader(unsigned int program, unsigned int shader) {
    // Make sure compile status was checked and successful when attaching a shader.
    auto it = shaders.find(shader);
    if (it == shaders.end()) {
        output_fmt("glAttachShader(program = %u, shader = %u): Invalid shader handle.", program, shader);
        return;
    }

    if (it->second.compile_status == CompileStatus::UNCHECKED) {
        output_fmt("glAttachShader(program = %u, shader = %u): Always check shader compilation status before trying to use the object.", program, shader);
    } else if (it->second.compile_status == CompileStatus::FAILED) {
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

        it->second.link_status = static_cast<LinkStatus>(*params);
    }
}

void Context::glLinkProgram(GLuint program)
{
    auto program_it = programs.find(program);
    if (program_it == programs.end()) {
        output_fmt("glLinkProgram(program = %u): Invalid program handle.", program);
        return;
    }

    auto& programInfo = program_it->second;

    // Store all active uniforms in the program (location, array size, and type)
    GLint uniform_count{};
    gl.GetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
    if (uniform_count > 0)
    {
        GLint max_name_len{};
        gl.GetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        auto uniform_name = std::make_unique<char[]>(max_name_len);

        for (int i = 0; i < uniform_count; i++)
        {
            GLsizei uniform_name_length{};
            UniformInfo uniform_info{};
            gl.GetActiveUniform(
              program,
              i,
              max_name_len,
              &uniform_name_length,
              &uniform_info.array_size,
              &uniform_info.type,
              uniform_name.get());

            auto loc = gl.GetUniformLocation(program, uniform_name.get());

            programInfo.uniforms.emplace(loc, uniform_info);
        }
    }
}

void Context::glUseProgram(unsigned int handle) {
    if (handle == 0) {
        bound_program = 0;
        return;
    }

    if (!validate_program_status(handle)) {
        return;
    }

    // TODO: add optional performance warning for rebinding the same program
    //if (handle == bound_program) {
    //    output_fmt("glUseProgram(program = %u): Program is already bound.", handle);
    //}

    bound_program = handle;
}

void Context::glDeleteProgram(GLuint program)
{
    auto it = programs.find(program);
    if (it == programs.end()) {
        output_fmt("glUseProgram(program = %u): Invalid program handle.", program);
        return;
    }

    // note: this does not change which program is bound, even if it becomes invalid here

    programs.erase(it);
}

void Context::validate_program_bound(std::string_view func_name)
{
    if (bound_program == 0) {
        output_fmt("%s: No program bound.", func_name.data());
        return;
    }
}

bool Context::validate_program_status(GLuint program)
{
    auto it = programs.find(program);
    if (it == programs.end()) {
        output_fmt("glUseProgram(program = %u): Invalid program handle.", program);
        return false;
    }

    if (it->second.link_status == LinkStatus::UNCHECKED) {
        output_fmt("glUseProgram(program = %u): Always check program link status before trying to use the object.", program);
        return false;
    }

    if (it->second.link_status == LinkStatus::FAILED) {
        output_fmt("glUseProgram(program = %u): Program has a linker error.", program);
        return false;
    }

    return true;
}
    }
}

}