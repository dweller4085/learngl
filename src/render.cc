
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "render.hh"

function t_node::render(t_camera __in * camera) -> void {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader);
    
    glUniformMatrix4fv(
        glGetUniformLocation(shader, "mvp"),
        1,
        GL_FALSE,
        glm::value_ptr(camera->projection * camera->view * world_transform)
    );
    
    glBindVertexArray(mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.length());
}

function t_scene::render(t_camera __in * camera) -> void {
    for (int i = 0; i < nodes.length(); i += 1) {
        nodes[i].render(camera);
    }
}
