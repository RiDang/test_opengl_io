#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;


out vec3 arg_world_coord;
out vec3 arg_world_normal;
out vec3 arg_normal;
out vec2 arg_tex_coord;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_model_mat;

void main()
{
    arg_world_coord = vec3(normal_model_mat * vec4(in_pos, 1.0));
    arg_world_normal = vec3(normal_model_mat * vec4(in_normal, 1.0));
    arg_tex_coord = in_tex_coord;

    gl_Position = projection_mat * view_mat * model_mat * vec4(in_pos, 1.0);
    // gl_Position = vec4(in_pos, 1.0);
}