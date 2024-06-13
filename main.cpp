#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "shader/shader.h"
#include "camera/camera.h"
// #include "texture/texture.h"
#include "io/model.h"


typedef struct {
    glm::dvec2 last_down;
    bool is_last_down{false};
} MouseInfo;


const int kWidth = 800, kHeight = 600;
MouseInfo mouse_left_info, mouse_right_info;
Camera camera;

float delta_time = 0.5f;
float last_time = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset ){
    camera.update_forward(yoffset);
    camera.info();
}


void processInput(GLFWwindow* window){

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ){
        glfwSetWindowShouldClose(window, true);
    }
    // float  speed = 0.05f;;
    float  speed = delta_time * 10;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.update_forward(speed);
        camera.info();

    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.update_forward(speed);
        camera.info();

    }
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.update_camera(0, -speed);
        camera.info();
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.update_camera(0, speed);
        camera.info();
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        camera.reset();
        camera.info();
    }

    // ====== 鼠标右键设置 ===========


    auto process_mouse_key = [&](unsigned int left_or_right, MouseInfo& mouse_info){
        if(glfwGetMouseButton(window, left_or_right) == GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            glm::dvec2 cur_pos = glm::vec2(x, y);
            if(mouse_info.is_last_down){
                glm::dvec2 delta_pos = cur_pos - mouse_info.last_down;
                delta_pos *= 0.1;
                // camera.update_camera((float)(delta_pos[1]), (float)(delta_pos[0]));
                if(left_or_right == GLFW_MOUSE_BUTTON_RIGHT){
                    camera.update_camera((float)(delta_pos[1]), (float)(delta_pos[0]));
                }
                else{
                    camera.update_object((float)(delta_pos[1]), (float)(delta_pos[0]));
                }

            }
            else{
                mouse_info.is_last_down = true;
            }
            mouse_info.last_down = cur_pos;

        }

        if(glfwGetMouseButton(window, left_or_right) == GLFW_RELEASE){
            mouse_info.is_last_down = false;
        }
    };

    process_mouse_key(GLFW_MOUSE_BUTTON_RIGHT, mouse_right_info);
    process_mouse_key(GLFW_MOUSE_BUTTON_LEFT, mouse_left_info);

    // ====== 鼠标左设置 ===========
    
}

void show_mat4(const glm::mat4& mat4){
    std::cout << "[";
    for(size_t i=0; i<4; i++){
        std::cout << "[";
        for(size_t j=0; j<4; j++){
            if(j+1 < 4)
              std::cout << mat4[j][i] << ", ";
            else
              std::cout << mat4[j][i];
        }

        if(i < 3)
            std::cout << "]," << std::endl;
        else
            std::cout << "]";
    }
    std::cout << "]" << std::endl;

}

std::string get_root_path(){
    return "/home/ubt22/Projects/test_opengl_io/";
}

GLFWwindow* InitWindow(){
    // ============== 窗口初始化 start

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD " << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

Shader::PathMap get_path_map(const std::string& prefix){

    const std::string ROOT_PATH = get_root_path();
    const std::string vertex_path = ROOT_PATH + "/shader/" + prefix + "_shader_vertex.vs";
    const std::string frag_path = ROOT_PATH + "/shader/"+ prefix + "_shader_fragment.fs";
    Shader::PathMap path_map{{"vertex",vertex_path},
                            {"frag",frag_path}};
    return path_map;
}

void process_node(const aiNode* cur_node, const aiScene* scene){
    std::cout << " = " << cur_node << " mesh count " << cur_node->mNumMeshes << std::endl;
    std::cout << " = " << cur_node << " children count " << cur_node->mNumChildren << std::endl;

    for(size_t i=0; i< cur_node->mNumChildren; i++){
        process_node(cur_node->mChildren[i], scene);
    }
    
}


int main()
{



    // ============== 窗口初始化 end

    GLFWwindow* window = InitWindow();
    glGetError(); 


    const std::string img_path = get_root_path() + "/data/nanosuit/nanosuit.obj";
    Model in_model(img_path);

    in_model.setup_mesh();
    // Mesh& mesh0 = in_model.meshes_[0];
    // for(size_t i=0; i < mesh0.vertices_.size(); i++){
    //     std::cout << " - i " << i << ": " << mesh0.vertices_[i].pos.x << ", " << mesh0.vertices_[i].pos.y << ", " <<  mesh0.vertices_[i].pos.z << std::endl;
    //     std::cout << " - i " << i << ": " << mesh0.vertices_[i].normal.x << ", " << mesh0.vertices_[i].normal.y << ", " <<  mesh0.vertices_[i].normal.z << std::endl;
    //     std::cout << " - i " << i << ": " << mesh0.vertices_[i].tex_coord.x << ", " << mesh0.vertices_[i].tex_coord.y << std::endl;
    // }

    // for(size_t i=0; i<mesh0.indices_.size() - 3; i++){

    //     std::cout << mesh0.indices_[i] << ", " << mesh0.indices_[i+1] << ", " << mesh0.indices_[i+2] << std::endl;
    // }

    // exit(-1);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
        1.0f, 0.0f, 0.0f, 0, 0,
         0.5f, -0.5f, 0.0f, // right 
         0.0f, 1.0f, 0.0f, 1, 0,
         0.5f,  0.5f, 0.0f,  // top   
         0.0f, 0.0f, 1.0f, 1, 1,
         -0.5f,  0.5f, 0.0f,  // top   
         1.0f, 0.0f, 1.0f, 0, 1
    }; 

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    
    std::vector<Vertex> vertex_list;
    std::vector<unsigned> index_list {0, 1, 2, 0, 2, 3};
    for(size_t i=0; i< sizeof(vertices); i++){
        Vertex a_vertex;
        a_vertex.pos = glm::vec3(vertices[i*8 + 0], vertices[i*8 + 1], vertices[i*8 + 2]);
        a_vertex.normal = glm::vec3(vertices[i*8 + 3], vertices[i*8 + 4], vertices[i*8 + 5]);
        a_vertex.tex_coord = glm::vec2(vertices[i*8 + 6], vertices[i*8 + 7]);
        vertex_list.push_back(a_vertex);
    }

    Mesh a_mesh;
    a_mesh.vertices_ = vertex_list;
    a_mesh.indices_ = index_list;
    a_mesh.setup_mesh();

    
    glEnable(GL_DEPTH_TEST);
    Shader::PathMap object_ath_map = get_path_map("object");
    Shader object_shader(object_ath_map);

    // Shader::PathMap light_path_map = get_path_map("light");
    // Shader light_shader(light_path_map);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------


    // object_shader.use();
    // object_shader.set_int("texture_color", 0);
    // object_shader.set_int("texture_specular", 1);


    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::vec3 light_pos(10.0f, 10.0f, 10.0f);
    glm::vec3 light_color(1.0f, 1.0f, 1.0f);
    glm::vec3 object_color(1.0f, 0.5f, 0.3f);
    
    camera.camera_pos_ = glm::vec3(0.0f, 5.0f, 10.0f);
    camera.update_forward(0, 0);

    while(!glfwWindowShouldClose(window)){


        float cur_time = glfwGetTime();
        delta_time = cur_time - last_time;
        last_time = cur_time;

        glfwSetScrollCallback(window, scroll_callback);

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          =glm::mat4(1.0f);
        glm::mat4 projection    =glm::mat4(1.0f);
        glm::mat4 normal_model_mat = glm::mat4(1.0f);

        model  = glm::rotate(model, cur_time, glm::vec3(0.0f, 1.0f, 0.0f));
        normal_model_mat = glm::transpose(glm::inverse(model));
        view = camera.view_mat4_;
        projection = glm::perspective(glm::radians(90.0f), (float)kWidth / (float)kHeight, 0.1f, 200.0f);

        object_shader.use();
        object_shader.set_mat4("model_mat", glm::value_ptr(model));
        object_shader.set_mat4("view_mat", glm::value_ptr(view));
        object_shader.set_mat4("projection_mat", glm::value_ptr(projection));
        object_shader.set_mat4("normal_model_mat", glm::value_ptr(normal_model_mat));

        object_shader.set_vec3("light_pos", glm::value_ptr(light_pos));
        object_shader.set_vec3("camera_pos", glm::value_ptr(camera.camera_pos_));

        // a_mesh.draw(object_shader);

        in_model.draw(object_shader);
        // in_model.meshes_[1].draw(object_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // start = std::chrono::high_resolution_clock::now();
    }
    glfwTerminate();
    return 0;
}

