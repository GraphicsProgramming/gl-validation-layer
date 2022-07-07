#include <iostream>

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
    std::cout << "OpenGL Error (default validation)" << type << " message: " << message << std::endl;
}

unsigned int create_shader(const char* vtx_path, const char* frag_path) {
    using namespace std::literals::string_literals;

    std::fstream file(vtx_path);

    if (!file.good()) {
        return 0;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string vtx_source(buf.str());

    file.close();
    buf = std::stringstream{}; // reset buffer
    file.open(frag_path);

    if (!file.good()) {
        return 0;
    }

    buf << file.rdbuf();

    std::string frag_source(buf.str());
    buf = std::stringstream{};

    unsigned int vtx_shader = 0, frag_shader = 0;
    vtx_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // This is wrapped inside a lambda to limit the scope of vtx_carr and
    // frag_carr
    [&vtx_source, &frag_source, &vtx_shader, &frag_shader]() {
        const char* vtx_carr = vtx_source.c_str();
        const char* frag_carr = frag_source.c_str();
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
        return 0;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        std::cout << "fragment shader compilation failed: " << infolog << std::endl;
        return 0;
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
        return 0;
    }

    // These are linked now and can safely be deleted
    glDeleteShader(vtx_shader);
    glDeleteShader(frag_shader);

    return shaderProgram;
}

int main() {

    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Validation Layer - tests", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    // Load GLAD.
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Initialize our library.
    int error = gl_layer_init(3, 3);
    if (error) {
        std::cerr << "Could not initialize OpenGL Validation Layer\n";
        return -1;
    }

    // Note that this disables glDebugMessage
    glad_set_post_callback(&gl_layer_callback);

    // Main application loop

    unsigned int program = create_shader("shaders/vert.glsl", "shaders/frag.glsl");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glfwSwapBuffers(window);
    }

    // Cleanup
    gl_layer_terminate();
    glfwTerminate();
}