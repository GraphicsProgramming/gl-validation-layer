#include <iostream>

#include <gl_layer/context.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
    gl_layer_init(3, 3);
    glad_set_pre_callback(&gl_layer_callback);

    // Main application loop

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cleanup
    gl_layer_terminate();
    glfwTerminate();
}