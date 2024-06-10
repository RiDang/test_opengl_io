#include "./shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


/// @brief Shader 初始化—————在这里初始化不是好策略，后调整到init 中；
/// @param path_map 
Shader::Shader(const PathMap& path_map){
    // - 读取 shader 源文件
    std::cout << "Read: " << path_map.at("vertex") << std::endl;
    const std::string vertex_source = read_file(path_map.at("vertex"));
    const char* c_vertex_source = vertex_source.c_str();

    std::cout << "Read: " << path_map.at("frag") << std::endl;
    const std::string frag_source = read_file(path_map.at("frag"));    
    const char* c_frag_source = frag_source.c_str();

    // const char* c_vertex_source = "#version 330 core\n"
    //                             "layout (location = 0) in vec3 aPos;\n"
    //                             "void main()\n"
    //                             "{\n"
    //                             "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    //                             "}\0";

    // const char* c_frag_source = "#version 330 core\n"
    //                             "out vec4 FragColor;\n"
    //                             "void main()\n"
    //                             "{\n"
    //                             "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    //                             "}\0";

    int success;
    char infoLog[512];
    // - vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &c_vertex_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << " vertex_shader compile fail, info " << infoLog << std::endl;
        exit(-1);
    }
 

    // - fragement shader
    unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &c_frag_source, NULL);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        std::cout << " frag_shader compile fail, info " << infoLog << std::endl;
        exit(-1);
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << " shader_program compile fail, info " << infoLog << std::endl;
        exit(-1);
    }

    shader_program_ = shader_program;   

    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
}

std::string Shader::read_file(const std::string& path){
    std::ifstream fp;
    fp.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try{
        fp.open(path);
        std::stringstream shader_str;
        shader_str << fp.rdbuf();
        fp.close();
        return shader_str.str();

    }
    catch(std::ifstream::failure& e){
        std::cout << "ERROR: read shader file fail, info " << e.what() << std::endl;
    }
}



void Shader::use(){
    glUseProgram(shader_program_);
}


Shader::~Shader(){
    // glDeleteProgram(shader_program_); // segment_fault 
}

void Shader::set_bool(const std::string& name, const bool value){

}

void Shader::set_int(const std::string& name, const int value){
    glUniform1i(glGetUniformLocation(shader_program_, name.c_str()), value);
}

void Shader::set_float(const std::string& name, const float value){
    glUniform1f(glGetUniformLocation(shader_program_, name.c_str()), value);
}

void Shader::set_mat4(const std::string& name, const float* mat4){
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, name.c_str()), 1, GL_FALSE, mat4);
}

void Shader::set_vec3(const std::string& name, const float* vec3){
    glUniform3fv(glGetUniformLocation(shader_program_, name.c_str()), 1, vec3);
}