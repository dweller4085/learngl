#ifndef __learngl__
#define __learngl__

#include <mkb/base.hh>

struct vec2 {
    union {
        struct { float x, y; };
        struct { float s, t; };
        float p [2];
    };
};

struct vec3 {
    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
        float p [3];
    };
};

struct vec4 {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        float p [4];
    };
};

struct t_vertex {
    vec3 pos;
    vec3 color;
    vec2 tex;
};

#endif // __learngl__
