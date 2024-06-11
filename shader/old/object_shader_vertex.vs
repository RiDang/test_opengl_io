#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;


out vec3 arg_world_pos;
out vec3 arg_world_normal;
out vec2 arg_tex_coord;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

uniform mat4 normal_model_mat;

void main()
{
   arg_world_pos = vec3(model_mat * vec4(in_pos, 1.0f));
   arg_world_normal = mat3(normal_model_mat) * in_normal;
   arg_tex_coord = in_tex_coord;
   // arg_world_normal = mat3(transpose(inverse(view_mat * model_mat))) * in_normal;
   gl_Position = projection_mat * view_mat * vec4(arg_world_pos, 1.0);
   
}