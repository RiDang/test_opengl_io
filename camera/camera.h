#ifndef OPENGL_CAMERA_H_
#define OPENGL_CAMERA_H_

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera{
public:
    Camera();
    ~Camera();

    int init();

    /**
     * 根据输入进行计算
     *@ parameters: pitch: x/z - y,  yaw: x - y,  
    */ 
    void compute_camera_mat4();

    /**
     * 进行连续更新
    */

   void update_camera(const float delta_pitch, const float delta_yaw, bool constrain = true);

   void update_object(const float delta_phi, const float delta_theta, bool constrain = true);

   void update_forward(const float delta_forward, bool constrain = true);

    void compute_object_mat4();

    glm::mat4 get_object_mat4();

    void info() const;
    
    void reset();

    glm::mat4 look_at(const glm::vec3& start_pos, const glm::vec3& end_pos, const glm::vec3& up);


public:
    glm::vec3 camera_pos_{0.0f, 0.0f, 1.0f};
    glm::vec3 camera_front_{0.0f, 0.0f, -1.0f};
    glm::vec3 camera_up_{0.0f, 1.0f, 0.0f};
    glm::vec3 camera_right{1.0f, 0.0f, 0.0f};
    const glm::vec3 kWorldUp_{0.0f, 1.0f, 0.0f};
    glm::mat4 view_mat4_{glm::mat4(1.0f)};
    glm::mat4 object_mat4_{glm::mat4(1.0f)};
    glm::mat4 camera_mat4_{glm::mat4(1.0f)};

    float pitch_{0.0f};
    float yaw_{0.0f};
    
    float phi_{0.0f}; 
    float theta_{0.0f};



    const float kSensitive{0.1}; // 鼠标
    const float kSpeed{2.5}; // 按键    
};


Camera::Camera(){
    // 计算相机姿态
    compute_camera_mat4();

    // 计算计算物体姿态
    compute_object_mat4();

    view_mat4_ = camera_mat4_ * object_mat4_;
}

Camera::~Camera(){}

void Camera::info() const {
    std::cout << " - camera_pos_ " << camera_pos_.x << ", " << camera_pos_.y << ", " << camera_pos_.z << std::endl;
}


void Camera::compute_object_mat4(){
    glm::mat4 object_mat4 = glm::mat4(1.0f);
    glm::vec3 axis_y =glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 axis_x = glm::vec3(cos(theta_), 0, -sin(theta_));
    object_mat4 = glm::rotate(object_mat4, glm::radians(theta_), axis_y); // y -> x
    object_mat4_ = glm::rotate(object_mat4, glm::radians(phi_), axis_x); // y -> x
}

glm::mat4 Camera::get_object_mat4(){
    return object_mat4_;
}

void Camera::reset(){
    camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    camera_pos_ = glm::vec3(0.0f, 0.0f, 1.0f);
    camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    pitch_ = 0.0f;
    yaw_ = 0.0f;
    phi_ = 0.0f;
    theta_ = 0.0f;
    
    compute_camera_mat4();
    compute_object_mat4();
    view_mat4_ = camera_mat4_ * object_mat4_;
    
}


void Camera::compute_camera_mat4(){
    
    glm::vec3 direction;
    // direction.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    // direction.y = sin(glm::radians(pitch_));
    // direction.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

    direction.z = -cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    direction.y = sin(glm::radians(pitch_));
    direction.x = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));


    camera_front_ = glm::normalize(direction);

    glm::vec3 camera_right_ = glm::normalize(glm::cross(camera_front_, kWorldUp_));
    glm::vec3 camera_up_ = glm::normalize(glm::cross(camera_right_, camera_front_));

    camera_mat4_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
}

void Camera::update_camera(const float delta_pitch, const float delta_yaw, bool constrain){
    // 首先进行更新
    pitch_ += delta_pitch;
    yaw_ += delta_yaw;

    // 边界约束
    if(constrain){
        if (pitch_ > 89.0){ pitch_ = 89.0;}
        else if(pitch_ < -89.0){pitch_ = -89.0;}

        if (yaw_ > 89.0){ yaw_ = 89.0;}
        else if(yaw_ < -89.0){yaw_ = -89.0;}
    }

    // 计算相机姿态
    compute_camera_mat4();

    // 计算旋转矩阵
   view_mat4_ = camera_mat4_ * object_mat4_;
}

void Camera::update_object(const float delta_phi, const float delta_theta, bool constrain){
    // 首先进行更新
    phi_ += delta_phi;
    theta_ += delta_theta;

    // 边界约束
    if(constrain){
        if (phi_ > 89.0){ phi_ = 89.0;}
        else if(phi_ < -89.0){phi_ = -89.0;}

        if (theta_ > 89.0){ theta_ = 89.0;}
        else if(theta_ < -89.0){theta_ = -89.0;}
    }

    // 计算相机姿态
    compute_object_mat4();
    
    // 计算旋转矩阵
   view_mat4_ = camera_mat4_ * object_mat4_;
}




void Camera::update_forward(const float delta_forward, bool constrain){
     camera_pos_ -= delta_forward * camera_front_;
     camera_mat4_ = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);

     view_mat4_ = camera_mat4_ *  object_mat4_;

}


glm::mat4 Camera::look_at(const glm::vec3& start_pos, const glm::vec3& end_pos, const glm::vec3& up){
    glm::vec3 direction = glm::normalize(end_pos - start_pos);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    glm::vec3 ortho_up = glm::normalize(glm::cross(right, direction));


    glm::mat4 target_view = glm::mat4(1.0f);
    target_view[0] = glm::vec4(right, 0);
    target_view[1] = glm::vec4(ortho_up, 0);
    target_view[2] = glm::vec4(-direction, 0);
    target_view[3] = glm::vec4(start_pos, 1);

    return  glm::inverse(target_view);

}







#endif