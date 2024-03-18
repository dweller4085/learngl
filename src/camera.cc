
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "app.hh"

namespace {
    constexpr float pi =  3.14159265359f;
    constexpr float tau = 6.28318530717f;
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    
    inline function fps_view_transform(vec3 position, float pitch, float yaw) -> mat4 {
        let transform = glm::identity<mat4>();
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, 0.25f * tau, { 1.f, 0.f, 0.f });
        transform = glm::rotate(transform, yaw * tau, { 0.f, 1.f, 0.f });
        transform = glm::rotate(transform, pitch * tau, { 1.f, 0.f, 0.f });
        
        return glm::inverse(transform);
    }
}

function cursor::get_position() -> vec2 {
    double x, y;
    glfwGetCursorPos(app.window, &x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}

function cursor::get_delta() -> vec2 {
    vec2 static previous = cursor::get_position();
    vec2 static filter_buffer[2] = {};
    int static i = 0;
    
    let current = cursor::get_position();
    
    filter_buffer[i] = current - previous;
    
    previous = current;
    i = (i + 1) % 2;
    
    return (filter_buffer[0] + filter_buffer[1]) * 0.5f;
}

function t_camera::update(float dt) -> void {
    if (can_move) {
        { // update pitch and yaw angle values
            let delta = cursor::get_delta() * 0.0004f;
            delta.x *= sensitivity;
            delta.y *= 0.86f * sensitivity;
            
            yaw = std::fmodf(yaw - delta.x, 1.f);
            pitch = m_clamp(pitch - delta.y, -max_pitch, max_pitch);
        }
        
        { // update position
            let rot = glm::identity<mat4>();
            rot = glm::rotate(rot, yaw * tau, { 0.f, 0.f, 1.f });
            rot = glm::rotate(rot, pitch * tau, { 1.f, 0.f, 0.f });
            
            let front = vec3 { rot * vec4 { 0.f, 1.f, 0.f, 0.f } };
            let right = vec3 { rot * vec4 { 1.f, 0.f, 0.f, 0.f } };
            let up = vec3 { 0.f, 0.f, 1.f };
        
            let direction = glm::zero<vec3>();
            
            direction += front * (float) (glfwGetKey(app.window, GLFW_KEY_W) == GLFW_PRESS);
            direction -= front * (float) (glfwGetKey(app.window, GLFW_KEY_S) == GLFW_PRESS);
            direction += right * (float) (glfwGetKey(app.window, GLFW_KEY_D) == GLFW_PRESS);
            direction -= right * (float) (glfwGetKey(app.window, GLFW_KEY_A) == GLFW_PRESS);
            direction += up    * (float) (glfwGetKey(app.window, GLFW_KEY_SPACE) == GLFW_PRESS);
            direction -= up    * (float) (glfwGetKey(app.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
            
            let shift = glfwGetKey(app.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            
            if (glm::length(direction) > epsilon) {
                velocity += glm::normalize(direction) * accel_rate * dt;
            }
            
            let speed = glm::length(velocity);
            
            if (speed > epsilon) {
                velocity = glm::normalize(velocity) * m_clamp(speed - decel_rate * dt, 0.f, max_speed * (shift ? 0.5f : 1.f));
            }
            
            position += velocity * dt;
        }
    }
    
    view = fps_view_transform(position, pitch, yaw);
}
