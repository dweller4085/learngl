
#include <cstdio>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learngl.hh"

namespace {
    
    constexpr int screen_width = 640;
    constexpr int screen_height = 480;
    
    const char * vertex_shader_src = R"(
        #version 450 core
        
        layout (location = 0) in vec3 v_pos;
        layout (location = 1) in vec2 v_tex;
        out vec2 f_tex;
        uniform mat4 transform;
        
        void main() {
            gl_Position = transform * vec4(v_pos, 1.0);
            f_tex = v_tex;
        }
    )";
    
    const char * fragment_shader_src = R"(
        #version 450 core
        
        in vec2 f_tex;
        out vec4 color;
        uniform sampler2D f_texture;
        
        void main() {
            color = texture(f_texture, f_tex);
        }
    )";
    
    t_vertex vertices[] = {
        { .pos =  {  0.5f,  0.5f, 0.0f }, .tex = { 1.0f, 1.0f } },
        { .pos =  {  0.5f, -0.5f, 0.0f }, .tex = { 1.0f, 0.0f } },
        { .pos =  { -0.5f, -0.5f, 0.0f }, .tex = { 0.0f, 0.0f } },
        { .pos =  { -0.5f,  0.5f, 0.0f }, .tex = { 0.0f, 1.0f } },
    };
    
    uint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };
}

function main(int argc, char ** argv) -> int {
    
    m_assert(glfwInit());
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    let window = glfwCreateWindow(screen_width, screen_height, "learngl", null, null);
    
    glfwMakeContextCurrent(window);
    
    m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    
    uint shader_program = [] {
        
        let vertex_shader = [] {
            
            let vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_shader, 1, &vertex_shader_src, null);
            glCompileShader(vertex_shader);
            
            int ok = 0;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
            
            return vertex_shader;
        } ();

        let fragment_shader = [] {
            
            let fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment_shader, 1, &fragment_shader_src, null);
            glCompileShader(fragment_shader);
            
            int ok = 0;
            glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
            
            return fragment_shader;
        } ();
        
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
    uint ebo = 0;
    
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_vertex), (void *) 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(t_vertex), (void *) (1 * sizeof(vec3)));
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glBindVertexArray(0);
    }

    uint texture = [=] {
        uint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int width, height, n_channels;
        let data = stbi_load("w:\\learngl\\resources\\box.jpg", &width, &height, &n_channels, 0);
        m_assert(data);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glUniform1i(glGetUniformLocation(shader_program, "f_texture"), 0);
        
        stbi_image_free(data);
        
        return texture;
    } ();
    
    while (!glfwWindowShouldClose(window)) {
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        let transform = glm::mat4 { 1.0f };
        transform = glm::rotate(transform, (float) glfwGetTime(), glm::vec3 { 0.f, 0.f, 1.f });
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glUseProgram(shader_program);
        
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
    
    glfwTerminate();
    return 0;
}
