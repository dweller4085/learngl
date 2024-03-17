
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

using glm::vec2, glm::vec3, glm::vec4, glm::mat4;



function view_transform_test() {
    
    struct {
        float pitch = 0.f;
        float yaw = 0.125f;
        vec3 posisiton = { 0.f, 0.f, 0.f };
    } camera;
    
    vec4 v = { 0.f, 0.f, 1.f, 1.f };
    
    let rot = glm::identity<mat4>();
    rot = glm::rotate(rot, 0.25f * tau, vec3 { 1.f, 0.f, 0.f });
    rot = glm::rotate(rot, camera.pitch * tau, { 1.f, 0.f, 0.f });
    rot = glm::rotate(rot, camera.yaw * tau, { 0.f, 1.f, 0.f });
    
    let transform = rot;
    transform = glm::translate(transform, camera.posisiton);
    
    let view = glm::inverse(transform);
    
    v = view * v;
    
    std::printf("%f %f %f %f", v.x, v.y, v.z, v.w);
}

namespace {   
    struct t_camera {
        static constexpr float max_pitch = 0.25f - 1.f / 256.f;
        
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
        static constexpr vec3 world_right = { 1.f, 0.f, 0.f };
        static constexpr vec3 world_front = { 0.f, 1.f, 0.f };
        static constexpr vec3 world_up = { 0.f, 0.f, 1.f };
        
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
    
    inline function fps_view(vec3 position, float pitch, float yaw) -> mat4 {
        let transform = glm::identity<mat4>();
        transform = glm::rotate(transform, 0.25f * tau, vec3 { 1.f, 0.f, 0.f });
        transform = glm::rotate(transform, pitch * tau, { 1.f, 0.f, 0.f });
        transform = glm::rotate(transform, yaw * tau, { 0.f, 1.f, 0.f });
        transform = glm::translate(transform, position);
        
        return glm::inverse(transform);
    }
    
    t_app app;
}


function ::t_app::init() -> void {
    width = 640;
    height = 480;
    
    cursor.enabled = true;
    
    camera = {
        .position = { 0.f, 0.f, 4.f },
        .velocity = { 0.f, 0.f, 0.f },
        .pitch = 0.f,
        .yaw = 0.f,
        .max_speed = 8.f,
        .accel_rate = 80.f,
        .decel_rate = 120.f,
        .sensitivity = 2.f,
        .fov = 90.f,
    };
    
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
        app.width = width;
        app.height = height;
    });
    
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // let monitor = glfwGetPrimaryMonitor();
    // let mode = glfwGetVideoMode(monitor);
    // glfwSetWindowMonitor(window, monitor, 0, 0, screen_width, screen_height, mode->refreshRate);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    
    glEnable(GL_DEPTH_TEST);
}

function ::t_app::update(float dt) -> void {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    camera.update(dt);
}

function ::t_app::run() -> void {
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

function ::t_app::render() -> void {
    projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
    // ...
}

function ::t_app::terminate() -> void {
    glfwTerminate();
}

function ::t_cursor::get_position() -> vec2 {
    double x, y;
    glfwGetCursorPos(app.window, &x, &y);
    return { static_cast<float>(x), static_cast<float>(-y) };
}

function ::t_camera::update(float dt) -> void {
    { // update pitch and yaw angle values
        let static previous = t_cursor::get_position();
        let current = t_cursor::get_position();
        
        let delta = (previous - current) * sensitivity;
        
        previous = current;
        
        yaw = std::fmodf(yaw + delta.x, 1.f);
        pitch = m_clamp(pitch + delta.y, -max_pitch, max_pitch);
    }
    
    { // update position
        let rot = glm::identity<mat4>();
        rot = glm::rotate(rot, pitch * tau, { 1.f, 0.f, 0.f });
        rot = glm::rotate(rot, yaw * tau, { 0.f, 0.f, 1.f });
        
        let front = vec3 { rot * vec4 { 0.f, 1.f, 0.f, 0.f } };
        let right = vec3 { rot * vec4 { 1.f, 0.f, 0.f, 0.f } };
        let up = glm::cross(right, front);
    
        let direction = glm::zero<vec3>();
        
        direction += front * (float) (glfwGetKey(app.window, GLFW_KEY_W) == GLFW_PRESS);
        direction -= front * (float) (glfwGetKey(app.window, GLFW_KEY_S) == GLFW_PRESS);
        direction += right * (float) (glfwGetKey(app.window, GLFW_KEY_D) == GLFW_PRESS);
        direction -= right * (float) (glfwGetKey(app.window, GLFW_KEY_A) == GLFW_PRESS);
        direction += up    * (float) (glfwGetKey(app.window, GLFW_KEY_SPACE) == GLFW_PRESS);
        direction -= up    * (float) (glfwGetKey(app.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
        
        direction = (glm::length(direction) < epsilon) ? vec3 { 0.f } : glm::normalize(direction);
        
        velocity += direction * accel_rate * dt;
        
        let speed = glm::length(velocity);
        
        if (speed > epsilon) {
            if (glm::length(direction) < epsilon) {
                speed = m_clamp_below(speed - decel_rate * dt, 0.f);
            }
            
            velocity = glm::normalize(velocity) * m_clamp_above(speed, max_speed);
        }
        
        position += velocity * dt;
    }
    
    view = fps_view(position, pitch, yaw);
}

function main(int argc, char ** argv) -> int {
    app.init();
    app.run();
}
