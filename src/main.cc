
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mkb/base.hh>

constexpr int screen_width = 640;
constexpr int screen_height = 480;

namespace {
    const char * vertex_shader_src = R"(
        #version 450 core
        layout (location = 0) in vec3 pos;
        
        void main() {
            gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
        }
    )";
    
    const char * fragment_shader_src = R"(
        #version 450 core
        out vec4 color;
        
        void main() {
            color = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";
}

function main(int argc, char ** argv) -> int {
    m_assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    let window = glfwCreateWindow(screen_width, screen_height, "learngl", null, null);
    glfwMakeContextCurrent(window);
    
    m_assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };
    
    uint shader_program = [] {
        uint vertex_shader = 0; {
            vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_shader, 1, &vertex_shader_src, null);
            glCompileShader(vertex_shader);
            
            int ok = 0;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
        }

        uint fragment_shader = 0; {
            fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment_shader, 1, &fragment_shader_src, null);
            glCompileShader(fragment_shader);
            
            int ok = 0;
            glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
        }
        
        let shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        
        int ok = 0;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &ok);
        m_assert(ok);
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        
        return shader_program;
    } ();
    

    uint vbo = 0;
    uint vao = 0;
    
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glBindVertexArray(0);
    }
    
    while (!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
    
    glfwTerminate();
    return 0;
}
