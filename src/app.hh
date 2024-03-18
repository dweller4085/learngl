#ifndef __learngl_app__
#define __learngl_app__

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "camera.hh"
#include "render.hh"

struct t_app {
    static function on_key_event(GLFWwindow __in * window, int key, int scancode, int action, int mods) -> void;
    
    
    function init() -> void;
    function update(float dt) -> void;
    function render() -> void;
    function run() -> int;
    function terminate() -> int;
    
    t_camera camera;
    t_scene scene;
    
    GLFWwindow * window;
    int width;
    int height;
    
    vec3 background_color;
};

extern function main(int argc, char ** argv) -> int;

extern t_app app;

#endif // __learngl_app__
