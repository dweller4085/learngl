
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mkb/base.hh>

constexpr uint width = 640;
constexpr uint height = 480;

function main(int argc, char ** argv) -> int {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    let window = glfwCreateWindow(width, height, "LearnOpenGL", null, null);
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // ...
        return -1;
    }
    
    while (!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    std::printf("hello!!!\n");
    
    glfwTerminate();
    
    return 0;
}
