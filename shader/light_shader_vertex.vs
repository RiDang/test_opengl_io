#version 330 core
layout (location = 0) in vec3 in_pos;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;


void main()
{
   vec3 args_world_pos = vec3(model_mat * vec4(in_pos, 1.0f));
   gl_Position = projection_mat * view_mat * vec4(args_world_pos, 1.0);
   
}