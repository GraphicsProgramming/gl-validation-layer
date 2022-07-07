#ifndef GL_VALIDATION_LAYER_H_
#define GL_VALIDATION_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

int gl_layer_init(int gl_version_major, int gl_version_minor);
void gl_layer_terminate();

void gl_layer_callback(const char* name, void* func_ptr, int num_args, ...);

#ifdef __cplusplus
};
#endif

#endif