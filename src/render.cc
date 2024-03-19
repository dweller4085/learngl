
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb/stb_image.h>

#include "render.hh"

function t_node::render(t_camera __in * camera) -> void {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader);
    
    glUniformMatrix4fv(
        glGetUniformLocation(shader, "mvp"),
        1,
        GL_FALSE,
        glm::value_ptr(
            camera->projection *
            camera->view *
            glm::translate(glm::identity<mat4>(), position) *
            glm::mat4_cast(orientation)
        )
    );
    
    glBindVertexArray(mesh->vao);
    
    // if (mesh->indices.ptr) {
    //     glDrawElements(GL_TRIANGLES, mesh->indices.length(), GL_UNSIGNED_INT, 0);
    // } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.length());
    // }
}

function t_scene::render(t_camera __in * camera) -> void {
    for (int i = 0; i < nodes.length(); i += 1) {
        nodes[i].render(camera);
    }
}

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

function create_shader(char const * vertex, char const * fragment) -> t_shader {
    let vertex_shader = [=] {
        let vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex, null);
        glCompileShader(vertex_shader);
        
        int ok = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
        m_assert(ok);
        
        return vertex_shader;
    } ();
    
    let fragment_shader = [=] {
        let fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment, null);
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

function create_basic_shader() -> uint {
    char static const * vertex = R"(
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
    
    char static const * fragment = R"(
        #version 450 core
        
        in vec2 f_tex;
        
        out vec4 color;
        
        uniform sampler2D f_texture;
        
        void main() {
            color = texture(f_texture, f_tex);
        }
    )";
    
    return create_shader(vertex, fragment);
}

function create_box_mesh() -> t_mesh {
    t_vertex static vertices[36] = {
        
        // front face
        { { 0.5f, 0.5f, 0.5f }, { 0.f, 1.f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.f, 0.f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.f, 0.f } },
        
        { { 0.5f, 0.5f, 0.5f }, { 0.f, 1.f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.f, 0.f } },
        { { -0.5f, 0.5f, 0.5f }, { 1.f, 1.f } },
        
        
        // back face
        { { -0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.f, 0.f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        
        { { -0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        { { 0.5f, -0.5f, 0.5f }, { 1.f, 1.f } },
        
        
        // right face
        { { 0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.f, 0.f } },
        { { 0.5f, 0.5f, -0.5f }, { 1.f, 0.f } },
        
        { { 0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { 0.5f, 0.5f, -0.5f }, { 1.f, 0.f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.f, 1.f } },
        
        
        // left face
        { { -0.5f, 0.5f, 0.5f }, { 0.f, 1.f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.f, 0.f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        
        { { -0.5f, 0.5f, 0.5f }, { 0.f, 1.f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        { { -0.5f, -0.5f, 0.5f }, { 1.f, 1.f } },
        
        
        // top face
        { { 0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.f, 0.f } },
        { { -0.5f, 0.5f, 0.5f }, { 1.f, 0.f } },
        
        { { 0.5f, -0.5f, 0.5f }, { 0.f, 1.f } },
        { { -0.5f, 0.5f, 0.5f }, { 1.f, 0.f } },
        { { -0.5f, -0.5f, 0.5f }, { 1.f, 1.f } },
        
        
        // bottom face
        { { 0.5f, 0.5f, -0.5f }, { 0.f, 1.f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.f, 0.f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        
        { { 0.5f, 0.5f, -0.5f }, { 0.f, 1.f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.f, 1.f } },
        
    };
    
    uint32 static indices[] = {
        {}
    };
    
    uint vbo = 0;
    uint vao = 0;
    uint ebo = 0;
    
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        
        glBindVertexArray(vao);
        
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            
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
        .vbo = vbo,
        .ebo = ebo,
        .vertices = { .ptr = (t_vertex *) vertices, .len = 36 },
        .indices = {},
    };
}
