#include <iostream>
#include <string>
#include <optional>

#include <gl_layer/context.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sstream>
#include <fstream>

// Added as a reference, so we can see what errors are already reported.
void gl_error_callback([[maybe_unused]] GLenum source,
                       GLenum type,
                       [[maybe_unused]] GLuint id,
                       GLenum severity,
                       [[maybe_unused]] GLsizei length,
                       const GLchar* message,
                       [[maybe_unused]] const void* userParam) {
    std::cout << "OpenGL Error (default validation): " << type << " message: " << message << std::endl;
}

std::optional<std::string> load_file(std::string_view path)
{
  std::fstream file(path.data());

  if (!file.good()) {
    return std::nullopt;
  }

  std::stringstream buf;
  buf << file.rdbuf();

  return std::string(buf.str());
}

unsigned int create_shader(std::string_view vtx_source, std::string_view frag_source) {
    using namespace std::literals::string_literals;

    unsigned int vtx_shader = 0, frag_shader = 0;
    vtx_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // This is wrapped inside a lambda to limit the scope of vtx_carr and
    // frag_carr
    [&vtx_source, &frag_source, &vtx_shader, &frag_shader]() {
        const char* vtx_carr = vtx_source.data();
        const char* frag_carr = frag_source.data();
        glShaderSource(vtx_shader, 1, &vtx_carr, nullptr);
        glShaderSource(frag_shader, 1, &frag_carr, nullptr);
    }();

    glCompileShader(vtx_shader);
    glCompileShader(frag_shader);

    // As a diagnostic test, we will try using a shader without checking its info log.


    // Now, check for compilation errors
    int success;
    char infolog[512];
    glGetShaderiv(vtx_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vtx_shader, 512, nullptr, infolog);
        std::cout << "vertex shader compilation failed: " << infolog << std::endl;
//        return 0;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        std::cout << "fragment shader compilation failed: " << infolog << std::endl;
//        return 0;
    }

    // Finally, link the vertex and fragment shader together
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vtx_shader);
    glAttachShader(shaderProgram, frag_shader);
    glLinkProgram(shaderProgram);

    // Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
        std::cout << "shader link failed: " << infolog << "\n";
 //       return 0;
    }

    // These are linked now and can safely be deleted
    glDeleteShader(vtx_shader);
    glDeleteShader(frag_shader);

    return shaderProgram;
}

unsigned create_shader_from_files(std::string_view vtx_path, std::string_view frag_path)
{
  auto vtx_source = load_file(vtx_path);
  if (!vtx_source) return 0;

  auto frag_source = load_file(frag_path);
  if (!frag_source) return 0;

  return create_shader(*vtx_source, *frag_source);
}

int main() {

    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Validation Layer - tests", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    // Load GLAD.
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Initialize our library.
    ContextGLFunctions glFuncs;
    glFuncs.GetActiveUniform = glad_glGetActiveUniform;
    glFuncs.GetUniformLocation = glad_glGetUniformLocation;
    glFuncs.GetProgramiv = glad_glGetProgramiv;
    int error = gl_layer_init(4, 6, &glFuncs);
    if (error) {
        std::cerr << "Could not initialize OpenGL Validation Layer\n";
        return -1;
    }

//    glEnable(GL_DEBUG_OUTPUT);
//    glDebugMessageCallback(&gl_error_callback, nullptr);
    glad_set_post_callback(&gl_layer_callback);

    const char* vtx_source2 = R"(
#version 330 core

layout(location = 0) in vec3 iPos;

void main() {
    gl_Position = vec4(iPos, 1);
}
)";

    const char* frag_source2 = R"(
#version 330 core

uniform vec3 u_color;
uniform float u_alpha;

out vec4 o_color;

void main() {
    o_color = vec4(u_color, u_alpha);
}
)";

    unsigned int program = create_shader_from_files("shaders/vert.glsl", "shaders/frag.glsl");
    unsigned int program2 = create_shader(vtx_source2, frag_source2);

    // Main application loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUseProgram(program2);

        glfwSwapBuffers(window);
    }

    // Cleanup
    gl_layer_terminate();
    glfwTerminate();
}