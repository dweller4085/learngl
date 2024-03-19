
#include <cstdio>
#include <cmath>

#include "app.hh"

namespace {
    inline function now() -> float { return static_cast<float>(glfwGetTime()); }
    
    char const * tile_path = "w:\\learngl\\resources\\tile.jpg";
    char const * concrete_path = "w:\\learngl\\resources\\concrete.jpg";
    char const * paving_path = "w:\\learngl\\resources\\paving.jpg";
    char const * earth_path = "w:\\learngl\\resources\\earth.jpg";
    
    t_texture tile, concrete, paving, earth;
}

function t_app::init() -> void {
    width = 640;
    height = 480;
    
    background_color = { 0.2f, 0.2f, 0.2f };
    
    camera = {
        .position = { 0.f, -3.f, 0.f },
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
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);
    
    t_mesh static box = create_box_mesh();
    t_mesh static sphere = create_sphere_mesh(16, 16);
    uint static basic_shader = create_basic_shader();
    
    tile = create_texture(tile_path);
    concrete = create_texture(concrete_path);
    paving = create_texture(paving_path);
    earth = create_texture(earth_path);
    
    t_node static nodes[4] = {
        { .texture = tile },
        { .texture = concrete },
        { .texture = paving },
        {}
    };
    
    for (int i = 0; i < 3; i += 1) {
        float theta = i * tau / 3.f + kappa;
        float radius = 1.5f;
        
        vec2 xy = {
            std::cosf(theta),
            std::sinf(theta)
        };
        
        nodes[i].position = { xy * radius, 0.f };
        
        nodes[i].mesh= &box;
        nodes[i].shader = basic_shader;
        nodes[i].orientation = {};
    }
    
    nodes[3] = {
        .mesh = &sphere,
        .texture = earth,
        .shader = basic_shader,
        .position = {},
        .orientation = glm::angleAxis(0.15f, vec3 {0.f, 1.f, 0.f}),
    };
    
    
    scene = { .nodes = { .ptr = nodes, .len = sizeof(nodes) / sizeof(t_node) } };
}

function t_app::update(float dt) -> void {
    camera.update(dt);
    
    for (int i = 0; i < 3; i += 1) {
        float theta = i * tau / 3.f + kappa + 0.33f * ::now();
        float radius = 2.3f;
        
        vec2 xy = {
            std::cosf(theta),
            std::sinf(theta)
        };
        
        scene.nodes[i].position = { xy * radius, 0.f };
    }
    
    scene.nodes[0].orientation = glm::angleAxis(
        0.5f * ::now(),
        glm::normalize(vec3 { 0.2f, 0.4f, 0.7f })
    );
    
    scene.nodes[0].position = {
        scene.nodes[0].position.x,
        scene.nodes[0].position.y,
        0.5f * std::sinf(0.5f * ::now()),
    };
    
    scene.nodes[1].orientation = glm::angleAxis(
        -0.35f * ::now(),
        glm::normalize(vec3 { 0.f, 0.8f, 1.f })
    );
    
    let q = glm::angleAxis(
        pi * (0.5f * std::sinf(0.7f * ::now()) + 0.5f),
        vec3 { 0.f, 0.f, 1.f }
    );
    
    scene.nodes[2].orientation = glm::angleAxis(
        pi * (0.5f * std::sinf(-0.7f * ::now()) + 0.5f),
        q * vec3 { 1.f, 0.f, 0.f }
    ) * q;
    
    scene.nodes[3].orientation *= glm::angleAxis(
        0.33f * dt,
        vec3 { 0.f, 0.f, 1.f }
    );
}

function t_app::render() -> void {
    camera.projection = glm::perspective(glm::radians(camera.fov / 2.f), (float) width / (float) height, 0.1f, 100.0f);
    
    glClearColor(background_color.r, background_color.b, background_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    
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
