include(FetchContent)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.7
)
FetchContent_MakeAvailable(glfw)

add_library(gl_validation_layer_tests_glad STATIC)
target_sources(gl_validation_layer_tests_glad PRIVATE glad/src/glad.c)
target_include_directories(gl_validation_layer_tests_glad PUBLIC glad/include)

add_executable(gl_validation_layer_tests main.cpp)
target_link_libraries(gl_validation_layer_tests PRIVATE gl_validation_layer gl_validation_layer_tests_glad glfw)

file(GLOB TEST_SHADERS "shaders/*.glsl")
add_custom_command(
        TARGET gl_validation_layer_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/shaders
        DEPENDS ${TEST_SHADERS}
        COMMENT "OpenGL Validation Layer - Tests - Copying shaders"
        VERBATIM
)