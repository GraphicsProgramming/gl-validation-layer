#ifndef GL_VALIDATION_LAYER_H_
#define GL_VALIDATION_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ContextGLFunctions
{
  // TODO: replace the types in the parameters with proper platform-detected ones
  // TODO: evaluate whether * should be replaced with something like APIENTRYP in glad
  void (*GetActiveUniform)(unsigned, unsigned, int, int*, int*, unsigned*, char*);
  int (*GetUniformLocation)(unsigned, const char*);
  void (*GetProgramiv)(unsigned, unsigned, int*);
}ContextGLFunctions;

/**
 * @brief Initialize the OpenGL Validation Layer
 * @param gl_version_major OpenGL context major version.
 * @param gl_version_minor OpenGL context minor version.
 * @param gl_functions Structure with OpenGL function pointers the layer needs to call to work.
 * @return 0 on success, any other value on error.
 */
int gl_layer_init(unsigned int gl_version_major, unsigned int gl_version_minor, const ContextGLFunctions* gl_functions);

/**
 * @brief Terminate the OpenGL Validation Layer.
 */
void gl_layer_terminate();

/**
 * @brief This function will be used to perform validation. To enable validation, this must be called before every OpenGL call you make.
 *        When using the GLAD loader, this can be done by simply calling glad_set_pre_callback(&gl_layer_callback). If you are using a
 *        different loader, you will need to register this callback some other way.
 * @param name Name of the OpenGL function being called.
 * @param func_ptr Pointer to the OpenGL function being called.
 * @param num_args Amount of arguments.
 * @param ... Arguments to the OpenGL function being called.
 */
void gl_layer_callback(const char* name, void* func_ptr, int num_args, ...);

typedef void (*GLLayerOutputFun)(const char* text, void* user_data);
/**
 * @brief This function allows the user to set a custom callback for writing validation output.
 * @param callback Callback that is called when output needs to be written. This function must take an argument of type const char* with the text to write,
 *                 and a void* for user data.
 * @param user_data Pointer to any user data you want passed into the function.
 */
[[maybe_unused]] void gl_layer_set_output_callback(GLLayerOutputFun callback, void* user_data = nullptr);

#ifdef __cplusplus
};
#endif

#endif