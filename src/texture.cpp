#include <gl_layer/private/context.h>

namespace gl_layer {

void Context::glGenTextures(GLsizei count, GLuint* handles) {
    for (GLsizei i = 0; i < count; ++i) {
        textures.emplace(handles[i], Texture {
            handles[i],
            NO_TEXTURE_TARGET
        });
    }
}

void Context::glCreateTextures(GLenum target, GLsizei count, GLuint* handles) {
    for (GLsizei i = 0; i < count; ++i) {
        textures.emplace(handles[i], Texture {
           handles[i],
           static_cast<GLTextureTarget>(target)
        });
    }
}

void Context::glBindTexture(GLenum target, GLuint texture_handle) {
    if (texture_handle == 0) {
        // TODO: Discourage binding zero handle? (Suggest 1x1 texture to mark "invalid" instead).
        return;
    }

    auto it = textures.find(texture_handle);
    if (it == textures.end()) {
        output_fmt("glBindTexture(target = %s, texture = %u): Invalid texture handle.", enum_str(target), texture_handle);
        return;
    }
    Texture& texture = it->second;

    // Target not set yet, this is the first time this texture is bound.
    if (texture.target == NO_TEXTURE_TARGET) {
        texture.target = static_cast<GLTextureTarget>(target);
    }

    if (target != texture.target) {
        output_fmt("glBindTexture(target = %s, texture = %u): Invalid texture target (texture is already %s).", enum_str(target), texture_handle, enum_str(texture.target));
        return;
    }

    bound_textures[texture.target] = texture_handle;
}

void Context::glDeleteTextures(GLsizei count, GLuint* handles) {
    for (GLsizei i = 0; i < count; ++i) {
        textures.erase(handles[i]);
    }
}

}