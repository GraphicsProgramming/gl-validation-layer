#ifndef GL_VALIDATION_LAYER_H_
#define GL_VALIDATION_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

int gl_layer_init(unsigned int gl_version_major, unsigned int gl_version_minor);
void gl_layer_terminate();

void gl_layer_callback(const char* name, void* func_ptr, int num_args, ...);

typedef void (*GLLayerOutputFun)(const char* text);
void gl_layer_set_output_callback(GLLayerOutputFun callback);

#ifdef __cplusplus
};
#endif

#endif