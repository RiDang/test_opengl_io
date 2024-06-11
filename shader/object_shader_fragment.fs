#version 330 core
in vec3 arg_world_coord;
in vec3 arg_world_normal;
in vec2 arg_tex_coord;

out vec4 FragColor;



uniform sampler2D tex_diffuse1;
uniform sampler2D tex_specular1;
uniform sampler2D tex_normal1;
uniform sampler2D tex_height1;

uniform vec3 light_pos;
uniform vec3 camera_pos;



void main()
{    
    float ratio = 0.25;
    // FragColor = texture(texture_diffuse1, TexCoords);
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 light_dir = normalize(light_pos - arg_world_coord);
    vec3 camera_dir = normalize(camera_pos - arg_world_coord);


    vec3 diffusion = vec3(texture(tex_diffuse1, arg_tex_coord)) * ratio;
    diffusion *= max(dot(light_dir, normalize(arg_world_normal)), 0.0);

    vec3 specular = vec3(texture(tex_specular1, arg_tex_coord)) * ratio;
    vec3 ref_dir = reflect(light_dir, normalize(arg_world_normal));
    specular *= pow(dot(ref_dir, camera_dir), 32);


    
    FragColor = vec4(diffusion, 1.0) + vec4(specular, 1.0) + 
                texture(tex_normal1, arg_tex_coord) * ratio +
                texture(tex_height1, arg_tex_coord) * ratio;

}