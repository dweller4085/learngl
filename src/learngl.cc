
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

using glm::mat4, glm::vec3, glm::vec2;



namespace {   
    struct t_camera {
        constexpr float pitch_limit = 0.25f - 0.005f;
        
        function update(float dt) -> void;
        
        vec3 position;
        vec3 velocity;
        float pitch;
        float yaw;
        
        float max_speed;
        float accel_rate;
        float decel_rate;
        
        float sensitivity;
        float fov;
        
        mat4 view;
    };
    
    struct t_cursor {
        static function get_position() -> vec2;
        bool32 enabled;
    };
    
    struct t_app {
        function init() -> void;
        function update(float dt) -> void;
        function render() -> void;
        function run() -> void;
        function terminate() -> void;
        
        t_camera camera;
        t_cursor cursor;
        
        GLFWwindow * window;
        int width;
        int height;
        
        mat4 projection;
    };
    
    function t_app::init() -> void {
        width = 640;
        height = 480;
        
        cursor.enabled = true;
        cursor.position = [] {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            return glm::vec2 { .x = static_cast<float>(x), .y = static_cast<float>(-y) };
        } ();
        
        camera = {
            .position = { 0.f, 0.f, 4.f },
            .velocity = { 0.f, 0.f, 0.f },
            .pitch = 0.f,
            .yaw = 0.f,
            .max_speed = 8.f,
            .accel_rate = 80.f,
            .decel_rate = 120.f,
            .sensitivity = 2.f,
            .fov = 90.f;
        };
        
        projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
        
        m_assert(glfwInit());
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        window = glfwCreateWindow(width, height, "learngl", null, null);
        
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        
        glfwSetFramebufferSizeCallback(window, [] (GLFWwindow __in *, int width, int height) {
            glViewport(0, 0, width, height);
            self.width = width;
            self.height = height;
        });
        
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // let monitor = glfwGetPrimaryMonitor();
        // let mode = glfwGetVideoMode(monitor);
        // glfwSetWindowMonitor(window, monitor, 0, 0, screen_width, screen_height, mode->refreshRate);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        
        m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
        
        glEnable(GL_DEPTH_TEST);
    }
    
    function t_app::update(float dt) -> void {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        camera.update(dt);
    }
    
    function t_app::run() -> void {
        let t = static_cast<float>(glfwGetTime());
        let dt = 1.f / 60.f;
        
        while (!glfwWindowShouldClose(window)) {
            update(dt);
            render();
            
            glfwSwapBuffers(window);
            glfwPollEvents();
            
            let now = static_cast<float>(glfwGetTime());
            dt = t - now;
            t = now;
        }
        
        terminate();
    }
    
    function t_app::render() -> void {
        projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glUseProgram(shader_program);
        
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
    }
    
    function t_app::terminate() -> void {
        glfwTerminate();
    }
    
    function t_cursor::get_position() -> vec2 {
        double x, y;
        glfwGetCursorPos(app.window, &x, &y);
        return { .x = static_cast<float>(x), .y = static_cast<float>(-y) };
    }
    
    function t_camera::update(float dt) -> void {
        
        {
            let static previous = t_cursor::get_position();
            let current = t_cursor::get_position();
            
            let delta = (previous - current) * camera.sensitivity;
            
            previous = current;
            
            yaw += std::fmodf(delta.x, 1.f);
            pitch = m_clamp(pitch + delta.y, -pitch_limit, pitch_limit);
        }
        
        let up = vec3 { 0.f, 0.f, 1.f };
        let right = vec3 { 1.f, 0.f, 0.f };
        
        
            
        
        let const right = glm::normalize(glm::cross(front, up));
        let dir = glm::vec3 { 0.f, 0.f, 0.f };
        
        dir += front * (float) (glfwGetKey(app.window, GLFW_KEY_W) == GLFW_PRESS);
        dir -= front * (float) (glfwGetKey(app.window, GLFW_KEY_S) == GLFW_PRESS);
        dir += right * (float) (glfwGetKey(app.window, GLFW_KEY_D) == GLFW_PRESS);
        dir -= right * (float) (glfwGetKey(app.window, GLFW_KEY_A) == GLFW_PRESS);
        
        dir = (glm::length(dir) == 0.f) ? glm::vec3 { 0.f } : glm::normalize(dir);
        
        velocity += dir * accel_rate * dt;
        
        let speed = glm::length(velocity);
        
        if (speed != 0.f) {
            if (glm::length(dir) == 0.f) {
                speed = m_clamp_below(speed - decel_rate * dt, 0.f);
            }
            
            velocity = glm::normalize(velocity) * m_clamp_above(max_speed, speed);
        }
        
        position += velocity * dt;
        
        view = glm::lookAt (
            position,
            position + front,
            up
        );
    }
    
    t_app app;
}

function main(int argc, char ** argv) -> int {
    app.init();
    app.run();
}
