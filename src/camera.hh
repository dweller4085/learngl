#ifndef __learngl_camera__
#define __learngl_camera__

#include "common.hh"

namespace cursor {
    function get_position() -> vec2;
    function get_delta() -> vec2;
};

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
    mat4 projection;
};

#endif // __learngl_camera__
