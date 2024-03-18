#ifndef __learngl_render__
#define __learngl_render__

#include "camera.hh"
#include "common.hh"

struct t_vertex {
    vec3 position;
    vec2 uv;
};

struct t_mesh {
    uint vao, vbo;
    t_slice<t_vertex> vertices;
};

struct t_node {
    function render(t_camera __in * camera) -> void;
    
    t_mesh * mesh;
    uint texture;
    uint shader;
    vec3 position;
    glm::quat orientation;
};

struct t_scene {
    function render(t_camera __in * camera) -> void;
    
    t_slice<t_node> nodes;
};

#endif // __learngl_render__
