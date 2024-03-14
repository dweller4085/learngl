
#include <cstdio>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include "learngl.hh"

namespace {
    constexpr int screen_width = 640;
    constexpr int screen_height = 480;
    
    char const * box_texture_path = "w:\\learngl\\resources\\tile.jpg";
    
    char const * vertex_shader_src = R"(
        #version 450 core
        
        layout (location = 0) in vec3 v_pos;
        layout (location = 1) in vec2 v_tex;
        
        out vec2 f_tex;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * model * vec4(v_pos, 1.0);
            f_tex = v_tex;
        }
    )";
    
    char const * fragment_shader_src = R"(
        #version 450 core
        
        in vec2 f_tex;
        
        out vec4 color;
        
        uniform sampler2D f_texture;
        
        void main() {
            color = texture(f_texture, f_tex);
        }
    )";
    
    float vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

        1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
    };
    
    constexpr int cube_count = 10;
    
    glm::vec3 cube_positions [cube_count] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };
    
    struct t_camera {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 front;
        glm::vec3 up;
        
        float pitch;
        float yaw;
        float speed;
        float accel;
        float decel;
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
    glfwSwapInterval(1);
    
    // let monitor = glfwGetPrimaryMonitor();
    // let mode = glfwGetVideoMode(monitor);
    // glfwSetWindowMonitor(window, monitor, 0, 0, screen_width, screen_height, mode->refreshRate);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    
    glEnable(GL_DEPTH_TEST);
    
    let shader_program = [] {
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
    
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (3 * sizeof(float)));
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glBindVertexArray(0);
    }
    
    let texture = [=] {
        uint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int width, height, n_channels;
        let data = stbi_load(box_texture_path, &width, &height, &n_channels, 0);
        m_assert(data);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glUseProgram(shader_program);
        glUniform1i(glGetUniformLocation(shader_program, "f_texture"), 0);
        
        stbi_image_free(data);
        
        return texture;
    } ();
    
    
    glUniformMatrix4fv (
        glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE,
        glm::value_ptr(glm::perspective(glm::radians(45.0f), (float )screen_width / (float) screen_height, 0.1f, 100.0f))
    );
    
    float dt = 1.f / 60.f;
    float pt = static_cast<float>(glfwGetTime());
    
    let camera = t_camera {
        .position = { 0.f, 0.f, 4.f },
        .velocity = { 0.f, 0.f, 0.f },
        .front = { 0.f, 0.f, -1.f },
        .up = { 0.f, 1.f, 0.f },
        .pitch = 0.f,
        .yaw = -1.f / 4.f,
        .speed = 6.f,
        .accel = 50.f,
        .decel = 40.f,
    };
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
            
            let dir = glm::vec3 { 0.f, 0.f, 0.f };
            
            dir += camera.front * (float) (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
            dir -= camera.front * (float) (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
            dir += glm::normalize(glm::cross(camera.front, camera.up)) * (float) (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
            dir -= glm::normalize(glm::cross(camera.front, camera.up)) * (float) (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
            
            dir = (glm::length(dir) == 0.f) ? glm::vec3 { 0.f } : glm::normalize(dir);
            
            camera.velocity += dir * camera.accel * dt;
            
            let speed = glm::length(camera.velocity);
            
            if (speed != 0.f) {
                if (glm::length(dir) == 0.f) {
                    speed = m_clamp_below(speed - camera.decel * dt, 0.f);
                }
                
                camera.velocity = glm::normalize(camera.velocity) * m_clamp_above(speed, camera.speed);
            }
            
            camera.position += camera.velocity * dt;
        }
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glUseProgram(shader_program);
        
        let view = glm::mat4 { 1.f };
        
        view = glm::lookAt (
            camera.position,
            camera.position + camera.front,
            camera.up
        );
        
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
        glBindVertexArray(vao);
        
        for (int i = 0; i < cube_count; i += 1) {
            let model = glm::mat4 { 1.0f };
            
            model = glm::translate(model, cube_positions[i]);
            model = glm::scale(model, { 0.5f, 0.5f, 0.5f });
            model = glm::rotate(model, glm::radians(20.0f * i), { 1.0f, 0.3f, 0.5f });
            model = glm::rotate(model, static_cast<float>(glfwGetTime()), { 0.f, 0.f, 1.f });
            
            glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
        
        let t = static_cast<float>(glfwGetTime());
        dt = t - pt;
        pt = t;
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
    
    glfwTerminate();
    return 0;
}
