#ifndef __learngl_mesh__
#define __learngl_mesh___

#include "common.hh"

struct t_mesh {
    uint vbo, vao, ebo;
    t_slice<vec3> points;
    t_slice<vec2> uv;
};

struct t_object {
    t_mesh mesh;
    mat4 transform;
};


#endif // __learngl_mesh__
