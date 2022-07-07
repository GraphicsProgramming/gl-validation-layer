#include <gl_layer/context.h>

#include <cstdio>

namespace gl_layer {

static void default_output_func(const char* text) {
    printf("%s", text);
}

struct Version {
    unsigned int major = 0;
    unsigned int minor = 0;
};

class Context {
public:
    explicit Context(Version version) : gl_version(version) {

    }

    void set_output_callback(GLLayerOutputFun callback) {
        output_fun = callback;
    }

private:
    Version gl_version;
    GLLayerOutputFun output_fun = &default_output_func;
};


namespace {
Context* g_context = nullptr;
}

} // namespace gl_layer

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
}

void gl_layer_set_output_callback(GLLayerOutputFun callback) {
    if (!gl_layer::g_context) {
        // Report error: context not initialized.
        return;
    }
    gl_layer::g_context->set_output_callback(callback);
}