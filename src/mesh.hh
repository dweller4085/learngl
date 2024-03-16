#ifndef __learngl_mesh__
#define __learngl_mesh___

#include <glm/glm.hpp>

struct t_mesh {
    uint vbo, vao, ebo;
    glm::vec3 points;
    glm::vec2 uv;
};

struct t_object {
    t_mesh mesh;
    glm::mat4 transform;
}


char const * box_texture_path = "w:\\learngl\\resources\\tile.jpg";

char const * vertex_shader_src = R"(
    #version 450 core
    
    layout (location = 0) in vec3 v_pos;
    layout (location = 1) in vec2 v_tex;
    
    out vec2 f_tex;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        gl_Position = projection * view * model * vec4(v_pos, 1.0);
        f_tex = v_tex;
    }
)";

char const * fragment_shader_src = R"(
    #version 450 core
    
    in vec2 f_tex;
    
    out vec4 color;
    
    uniform sampler2D f_texture;
    
    void main() {
        color = texture(f_texture, f_tex);
    }
)";

float vertices[] = {
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

    1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
};


function cube_stuff() {
    let shader_program = [] {
        let vertex_shader = [] {
            let vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_shader, 1, &vertex_shader_src, null);
            glCompileShader(vertex_shader);
            
            int ok = 0;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
            
            return vertex_shader;
        } ();
        
        let fragment_shader = [] {
            let fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment_shader, 1, &fragment_shader_src, null);
            glCompileShader(fragment_shader);
            
            int ok = 0;
            glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
            m_assert(ok);
            
            return fragment_shader;
        } ();
        
        let shader_program = glCreateProgram();
        
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        
        int ok = 0;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &ok);
        m_assert(ok);
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        
        return shader_program;
    } ();


    uint vbo = 0;
    uint vao = 0;

    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (3 * sizeof(float)));
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glBindVertexArray(0);
    }

    let texture = [=] {
        uint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int width, height, n_channels;
        let data = stbi_load(box_texture_path, &width, &height, &n_channels, 0);
        m_assert(data);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glUseProgram(shader_program);
        glUniform1i(glGetUniformLocation(shader_program, "f_texture"), 0);
        
        stbi_image_free(data);
        
        return texture;
    } ();

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
}


#endif // __learngl_mesh__
