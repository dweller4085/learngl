#ifndef __learngl_render__
#define __learngl_render__

#include "camera.hh"
#include "common.hh"

using t_texture = uint;
using t_shader = uint;

struct t_vertex {
    vec3 position;
    vec2 uv;
};

struct t_mesh {
    uint vao, vbo, ebo;
    t_slice<t_vertex> vertices;
    t_slice<uint32> indices;
};

struct t_node {
    function render(t_camera __in * camera) -> void;
    
    t_mesh * mesh;
    t_texture texture;
    t_shader shader;
    vec3 position;
    glm::quat orientation;
};

struct t_scene {
    function render(t_camera __in * camera) -> void;
    
    t_slice<t_node> nodes;
};

function create_texture(char const * path) -> t_texture;
function create_shader(char const * vertex, char const * fragment) -> t_shader;
function create_basic_shader() -> t_shader;
function create_box_mesh() -> t_mesh;
function create_icosahedron() -> t_mesh;

#endif // __learngl_render__
