
#include <cstdio>
#include <cmath>

#include <stb/stb_image.h>

#include "app.hh"

namespace {
    function create_texture(char const * path) -> uint {
        uint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int width, height, n_channels;
        let data = stbi_load(path, &width, &height, &n_channels, 0);
        m_assert(data);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
        
        return texture;
    }

    function create_simple_shader() -> uint {
        char static const * vertex_shader_src = R"(
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
        
        char static const * fragment_shader_src = R"(
            #version 450 core
            
            in vec2 f_tex;
            
            out vec4 color;
            
            uniform sampler2D f_texture;
            
            void main() {
                color = texture(f_texture, f_tex);
            }
        )";
        
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
        
        let shader_program = [=] {
            let shader_program = glCreateProgram();
            glAttachShader(shader_program, vertex_shader);
            glAttachShader(shader_program, fragment_shader);
            glLinkProgram(shader_program);
            
            int ok = 0;
            glGetProgramiv(shader_program, GL_LINK_STATUS, &ok);
            m_assert(ok);
            
            return shader_program;
        } ();
        
        glUseProgram(shader_program);
        glUniform1i(glGetUniformLocation(shader_program, "f_texture"), 0);
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        
        return shader_program;
    }

    function create_box_mesh() -> t_mesh {
        float static vertices[36 * 5] = {
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
        
        uint vbo = 0;
        uint vao = 0;
        
        {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            
            {
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_vertex), (void *) 0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(t_vertex), (void *) (1 * sizeof(vec3)));
                
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            
            glBindVertexArray(0);
        }
        
        return {
            .vao = vao,
            .vertices = { .ptr = (t_vertex *) vertices, .len = 36 },
        };
    }
    
    inline function now() -> float {
        return static_cast<float>(glfwGetTime());
    }
    
    char const * tile_path = "w:\\learngl\\resources\\tile.jpg";
    char const * concrete_path = "w:\\learngl\\resources\\concrete.jpg";
    char const * crate_path = "w:\\learngl\\resources\\crate.jpg";
    
    uint tile, concrete, crate;
}

function t_app::init() -> void {
    width = 640;
    height = 480;
    
    background_color = { 0.2f, 0.2f, 0.2f };
    
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
        .can_move = false,
        .view = {},
        .projection = {},
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
    
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // let monitor = glfwGetPrimaryMonitor();
    // let mode = glfwGetVideoMode(monitor);
    // glfwSetWindowMonitor(window, monitor, 0, 0, screen_width, screen_height, mode->refreshRate);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    m_assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    
    glEnable(GL_DEPTH_TEST);
    
    t_mesh static box = create_box_mesh();
    uint static simple_shader = create_simple_shader();
    
    tile = create_texture(tile_path);
    concrete = create_texture(concrete_path);
    crate = create_texture(crate_path);
    
    t_node static nodes[1] = {
        { .mesh = &box, .texture = tile, .shader = simple_shader, .world_transform = glm::identity<mat4>() },
        // { .mesh = &box, .texture = concrete, .shader = simple_shader, .world_transform = glm::identity<mat4>() },
        // { .mesh = &box, .texture = crate, .shader = simple_shader, .world_transform = glm::identity<mat4>() },
    };
    
    scene = { .nodes = { .ptr = nodes, .len = 1 } };
}

function t_app::update(float dt) -> void {
    camera.update(dt);
}

function t_app::render() -> void {
    camera.projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
    
    glClearColor(background_color.r, background_color.b, background_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.render(&camera);
    
    glfwSwapBuffers(window);
}

function t_app::run() -> int {
    let then = ::now();
    let dt = 1.f / 60.f;
    
    while (!glfwWindowShouldClose(window)) {
        update(dt);
        render();
        
        glfwPollEvents();
        
        let now = ::now();
        dt = now - then;
        then = now;
    }
    
    return terminate();
}

function t_app::terminate() -> int {
    glfwTerminate();
    
    return 0;
}

function t_app::on_key_event(GLFWwindow __in * window, int key, int scancode, int action, int mods) -> void {
    vec2 static last_pos = cursor::get_position();
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window, last_pos.x, last_pos.y);
        } else {
            last_pos = cursor::get_position();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        
        app.camera.can_move ^= 1;
    }
}

function main(int argc, char ** argv) -> int {
    app.init();
    return app.run();
}

t_app app;
