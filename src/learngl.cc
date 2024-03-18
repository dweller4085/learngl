
#include <cstdio>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include "common.hh"

namespace {
    
    constexpr float magic_mouse_sensitivity_constant = 0.0005f;
    
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
        
        bool32 can_move;
        
        mat4 view;
    };
    
    struct t_cursor {
        static function get_position() -> vec2;
        bool32 is_enabled;
    };
    
    struct t_app {
        static function on_key_event(GLFWwindow __in *, int key, int scancode, int action, int mods) -> void;
        
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
        
        // NOTE this is temporary as fuck
        uint texture;
        uint shader_program;
        uint vao;
    };
    
    inline function fps_view(vec3 position, float pitch, float yaw) -> mat4 {
        let transform = glm::identity<mat4>();
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, 0.25f * tau, { 1.f, 0.f, 0.f });
        transform = glm::rotate(transform, yaw * tau, { 0.f, 1.f, 0.f });
        transform = glm::rotate(transform, pitch * tau, { 1.f, 0.f, 0.f });
        
        return glm::inverse(transform);
    }
    
    t_app app;
}

function ::t_app::on_key_event(GLFWwindow __in * window, int key, int scancode, int action, int mods) -> void {
    vec2 static last_pos = t_cursor::get_position();
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (app.cursor.is_enabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window, last_pos.x, last_pos.y);
        } else {
            last_pos = t_cursor::get_position();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        
        app.cursor.is_enabled ^= 1;
        app.camera.can_move ^= 1;
    }
}

function ::t_app::init() -> void {
    width = 640;
    height = 480;
    
    cursor.is_enabled = false;
    
    camera = {
        .position = { 0.f, 0.f, 0.f },
        .velocity = { 0.f, 0.f, 0.f },
        .pitch = 0.f,
        .yaw = 0.f,
        .max_speed = 4.f,
        .accel_rate = 100.f,
        .decel_rate = 50.f,
        .sensitivity = 1.f,
        .fov = 90.f,
        .can_move = true,
        .view = fps_view({ 0.f, 0.f, 0.f }, 0.f, 0.f),
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
    
    glfwSetKeyCallback(window, t_app::on_key_event);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // let monitor = glfwGetPrimaryMonitor();
    // let mode = glfwGetVideoMode(monitor);
    // glfwSetWindowMonitor(window, monitor, 0, 0, screen_width, screen_height, mode->refreshRate);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    
    glEnable(GL_DEPTH_TEST);
    
    /////////////// NOTE this is temporary as fuck /////////////////
    
    char const * box_texture_path = "w:\\learngl\\resources\\tile.jpg";
    
    char const * vertex_shader_src = R"(
        #version 450 core
        
        layout (location = 0) in vec3 v_pos;
        layout (location = 1) in vec2 v_tex;
        
        out vec2 f_tex;
        
        uniform mat4 mvp;
        
        void main() {
            gl_Position = mvp * vec4(v_pos, 1.0);
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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    };
    
    shader_program = [=] {
        let vertex_shader = [=] {
            let vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_shader, 1, &vertex_shader_src, null);
            glCompileShader(vertex_shader);
            
            int ok = 0;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
            
            return vertex_shader;
        } ();
        
        let fragment_shader = [=] {
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
    vao = 0;
    
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (1 * sizeof(vec3)));
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glBindVertexArray(0);
    }
    
    texture = [=] {
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
}

function ::t_app::update(float dt) -> void {
    camera.update(dt);
}

function ::t_app::run() -> void {
    let then = static_cast<float>(glfwGetTime());
    let dt = 1.f / 60.f;
    
    while (!glfwWindowShouldClose(window)) {
        update(dt);
        render();
        
        glfwPollEvents();
        
        let now = static_cast<float>(glfwGetTime());
        dt = now - then;
        then = now;
    }
    
    terminate();
}

function ::t_app::render() -> void {
    projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glUseProgram(shader_program);
    
    glBindVertexArray(vao);
    
    let model = glm::identity<mat4>();
    // model = glm::scale(model, { 0.5f, 0.5f, 0.5f });
    model = glm::translate(model, { 0.f, 3.f, 0.f });
    
    let mvp = projection * camera.view * model;
    
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glfwSwapBuffers(window);
}

function ::t_app::terminate() -> void {
    glfwTerminate();
}

function ::t_cursor::get_position() -> vec2 {
    double x, y;
    glfwGetCursorPos(app.window, &x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}

function ::t_camera::update(float dt) -> void {
    if (!can_move) return;
    
    { // update pitch and yaw angle values
        let static previous = t_cursor::get_position();
        let current = t_cursor::get_position();
        
        let delta = (current - previous) * magic_mouse_sensitivity_constant * sensitivity;
        
        previous = current;
        
        // filter the delta
        int static i = 0;
        vec2 static filter_buffer[2] = {};
        
        filter_buffer[i] = delta;
        
        delta = {};
        delta += filter_buffer[0];
        delta += filter_buffer[1];
        delta /= 2.f;
        
        i = (i + 1) % 2;
        
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
        
        if (glm::length(direction) > epsilon) {
            velocity += glm::normalize(direction) * accel_rate * dt;
        }
        
        let speed = glm::length(velocity);
        
        if (speed > epsilon) {
            velocity = glm::normalize(velocity) * m_clamp(speed - decel_rate * dt, 0.f, max_speed);
        }
        
        position += velocity * dt;
    }
    
    view = fps_view(position, pitch, yaw);
}

function main(int argc, char ** argv) -> int {
    app.init();
    app.run();
}

