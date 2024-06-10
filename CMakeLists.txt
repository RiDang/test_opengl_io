cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
 
project(test)
 
find_package(glfw3 REQUIRED)
find_package( OpenGL REQUIRED )
# include_directories( )
file(GLOB project_file  main.cpp
                        3rd/glad-3.3/src/glad.c 
                        shader/shader.cpp)
add_executable(${PROJECT_NAME} ${project_file})

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS} 
                                            "3rd/glad-3.3/include/"
                                            "shader/")
 
target_link_libraries(${PROJECT_NAME}  ${OPENGL_LIBRARIES} glfw dl)