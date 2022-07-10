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

void Context::glDeleteTextures(GLsizei count, GLuint* handles) {
    for (GLsizei i = 0; i < count; ++i) {
        textures.erase(handles[i]);
    }
}

}