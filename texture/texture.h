#ifndef OPENGL_TEXTURE_H_
#define OPENGL_TEXTURE_H_

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture{
public:
    Texture(const std::string& img_path);

    ~Texture();

    void use();

public:
    unsigned int texture_;
    unsigned char* img_data{nullptr};
};


Texture::Texture(const std::string& img_path){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理对象环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int color_format;
    if(img_path.rfind("jpg") != -1){
        color_format = GL_RGB;
    }
    else if(img_path.rfind("png") != -1){
        color_format = GL_RGBA;
    }else{
        std::cout << " error color_format " << img_path << std::endl;
        exit(-1);
    };

    // 加载纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load(img_path.c_str(), &width, &height, &nrChannels, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Read " << img_path << ", width "<< width << ", height " << height << ", nrChannels" << nrChannels << std::endl;
    }
    else
    {
        std::cout << "ERROR: Read image fail, path " << img_path << std::endl;
    }


    stbi_image_free(data);
    texture_ = texture;

}

Texture::~Texture(){
}

void Texture::use(){
    glBindTexture(GL_TEXTURE_2D, texture_);
}


#endif