#ifndef GL_VALIDATION_LAYER_TYPES_H_
#define GL_VALIDATION_LAYER_TYPES_H_

#include <vector>

namespace gl_layer {

using GLenum = unsigned int;

enum GLShaderInfoParam {
    GL_SHADER_TYPE = 0x8B4F,
    GL_DELETE_STATUS = 0x8B80,
    GL_COMPILE_STATUS = 0x8B81,
    GL_LINK_STATUS = 0x8B82,
    GL_INFO_LOG_LENGTH = 0x8B84,
    GL_SHADER_SOURCE_LENGTH = 0x8B88
};

enum class CompileStatus {
    UNCHECKED = -1,
    FAILED = 0,
    OK = 1,
};

enum class LinkStatus {
  UNCHECKED = -1,
  FAILED = 0,
  OK = 1,
};

const char* enum_str(GLenum v);

struct Version {
    unsigned int major = 0;
    unsigned int minor = 0;
};

// Represents a shader returned by glCreateShader
struct Shader {
    unsigned int handle {};
    // If this is -1, this means the compile status was never checked by the host application.
    // This is an error that should be reported.
    CompileStatus compile_status = CompileStatus::UNCHECKED;
};

// Represents a shader program returned by glCreateProgram
struct Program {
    unsigned int handle {};
    std::vector<unsigned int> shaders {};
    // If this is -1, this means the status was never checked by the host application.
    LinkStatus link_status = LinkStatus::UNCHECKED;
};

}

#endif