#ifndef GL_VALIDATION_LAYER_TYPES_H_
#define GL_VALIDATION_LAYER_TYPES_H_

#include <vector>
#include <unordered_map>
#include <cstdint>

namespace gl_layer {

using GLenum = std::uint32_t;
using GLuint = std::uint32_t;
using GLint = std::int32_t;
using GLsizei = std::int32_t;
using GLchar = char;
using GLfloat = float;
using GLboolean = bool;

enum GLShaderInfoParam {
    GL_SHADER_TYPE = 0x8B4F,
    GL_DELETE_STATUS = 0x8B80,
    GL_COMPILE_STATUS = 0x8B81,
    GL_LINK_STATUS = 0x8B82,
    GL_INFO_LOG_LENGTH = 0x8B84,
    GL_SHADER_SOURCE_LENGTH = 0x8B88,
    GL_ACTIVE_UNIFORM_MAX_LENGTH = 0x8B87,
    GL_ACTIVE_UNIFORMS = 0x8B86
};

enum GLTextureTarget {
    NO_TEXTURE_TARGET = 0x0,
    GL_TEXTURE_1D = 0x0DE0,
    GL_TEXTURE_2D = 0x0DE1,
    GL_TEXTURE_3D = 0x806F,
    GL_TEXTURE_1D_ARRAY = 0x8C18,
    GL_TEXTURE_2D_ARRAY = 0x8C1A,
    GL_TEXTURE_RECTANGLE = 0x84F5,
    GL_TEXTURE_CUBE_MAP = 0x8513,
    GL_TEXTURE_CUBE_MAP_ARRAY = 0x9009,
    GL_TEXTURE_BUFFER = 0x8C2A,
    GL_TEXTURE_2D_MULTISAMPLE = 0x9100,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102
};

enum GLFilter {
    GL_NEAREST = 0x2600,
    GL_LINEAR = 0x2601
};

enum GLEdgeSample {
    GL_REPEAT = 0x2901,
    GL_MIRRORED_REPEAT = 0x8370,
    GL_CLAMP_TO_EDGE = 0x812F,
    GL_CLAMP_TO_BORDER = 0x812D,
    // Core in 4.4
    GL_MIRROR_CLAMP_TO_EDGE = 0x8743
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

struct UniformInfo {
    GLint array_size;
    GLenum type;
    // std::string name // the name is probably not useful for us

    bool operator==(const UniformInfo& other) const {
      return array_size == other.array_size && type == other.type;
    }
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
    std::unordered_map<GLint, UniformInfo> uniforms {};
    // If this is -1, this means the status was never checked by the host application.
    LinkStatus link_status = LinkStatus::UNCHECKED;
};

// Represents either the sampler state of a texture, or a sampler object created by glGenSamplers()
struct Sampler {
    // GL_TEXTURE_MAG_FILTER
    GLFilter mag_filter {};
    // GL_TEXTURE_MIN_FILTER
    GLFilter min_filter {};
    // Core in 4.6
    // GL_TEXTURE_MAX_ANISOTROPY
    float anisotropy = 1.0;
    // GL_TEXTURE_MAX_LOD
    float max_lod = 0.0;
    // GL_TEXTURE_MIN_LOD
    float min_lod = 0.0;
    // GL_TEXTURE_LOD_BIAS
    float lod_bias = 0.0;
    // GL_TEXTURE_WRAP_S
    GLEdgeSample edge_s {};
    // GL_TEXTURE_WRAP_T
    GLEdgeSample edge_t {};
    // GL_TEXTURE_WRAP_R
    GLEdgeSample edge_r {};
    // GL_TEXTURE_BORDER_COLOR
    // From https://www.khronos.org/opengl/wiki/Sampler_Object#Sampling_parameters:
    // The border color can be provided in floating-point values, Normalized Integers, or non-normalized integers, using the various forms of glSamplerParameter/glTexParameter.
    // Note that the border color is a 4-component color, so you must use the v version of the function, and you must provide all four components in a single call.
    // When using the fv function, the color will be stored as a float. When using iv, the color will be converted to a float via signed normalization.
    // Since the components are GLint 32-bit integers, the range is from [-2^31, 2^31). When using the Ii or Iui forms, the color will be stored as signed or unsigned integers, as appropriate.
    // This means we can store the color as 4 floating point values, and convert them by normalizing in the gl[Texture/Sampler]Parameter* call.
    GLfloat border_color[4] {};
    // GL_TEXTURE_CUBE_MAP_SEAMLESS
    // May be overwritten by global parameter
    GLboolean cubemap_seamless = false;
};

// Represents a texture created by glGenTextures()
struct Texture {
    unsigned int handle {};
    // Texture target, either as specified by first glBindTexture() call, or by
    // glCreateTextures().
    GLTextureTarget target {};
};

}

#endif