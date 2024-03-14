#ifndef __learngl_mesh__
#define __learngl_mesh___

#include <glm/glm.hpp>

struct t_mesh {
    uint vbo, vao, ebo;
    glm::vec3 points;
    glm::vec2 uv;
};



#endif // __learngl_mesh__
