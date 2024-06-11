#ifndef OPENGL_IO_MODEL_H__
#define OPENGL_IO_MODEL_H__
#include <vector>
#include <map>
#include <unordered_map>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../io/stb_image.h"

#include "../shader/shader.h"

#define MAX_BONE_INFLUENCE 4

enum LightType{
    AMBIENT = 0,
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT
};
const std::string LightTypeStr(const LightType& light){
    switch(light){
        case LightType::AMBIENT:
            return "ambient";
        case LightType::DIFFUSE:
            return "diffuse";
        case LightType::SPECULAR:
            return "specular";
        case LightType::NORMAL:
            return "normal";
        case LightType::HEIGHT:
            return "height";
        default:
            std::cout << "ERROR: LightType " << LightType::AMBIENT << std::endl;
            return "";
    }
}


unsigned int load_texture(const std::string& img_path){

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    int start_id = img_path.find_last_of("/");
    std::string img_name = img_path.substr(start_id+1,  img_path.size() - start_id -1);

    // 设置纹理对象环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load(img_path.c_str(), &width, &height, &nrChannels, 0);
    
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Read " << img_path << ", width "<< width << ", height " << height << ", nrChannels" << nrChannels << std::endl;
    }
    else
    {
        std::cout << "ERROR: Read image fail, path " << img_path << std::endl;
    }

    stbi_image_free(data);

    return texture_id;

}



struct Texture{
    Texture(){}
    Texture(const unsigned int in_id, const std::string& in_type, const std::string& in_name):
        id(in_id), type(in_type), name(in_name){}
    unsigned int id;
    std::string type;
    std::string name;
};


struct Vertex{
    glm::vec3 pos{0.0f, 0.0f, 0.0f};
    glm::vec3 normal{0.0f, 0.0f, 0.0f};
    glm::vec2 tex_coord{0.0f, 0.0f};
    glm::vec3 tangent{0.0f, 0.0f, 0.0f};
    glm::vec3 bitangent{0.0f, 0.0f, 0.0f};
    int bone_ids[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
    
};

class Mesh{
public:
    Mesh(){};
    Mesh(const Mesh& other){
        vertices_ = other.vertices_;
        indices_ = other.indices_;
        textures_ = other.textures_;
    };
    Mesh(const Mesh&& other){
        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);
        textures_ = std::move(other.textures_);
    };
    ~Mesh(){
        // glDeleteVertexArrays(1, &VAO_);
        // glDeleteBuffers(1, &VBO_);
        // glDeleteBuffers(1, &EBO_);
    };

    void setup_mesh();
    void draw(Shader& shader);

public:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;
    unsigned int VBO_, EBO_, VAO_;
};

void Mesh::setup_mesh(){
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bone_ids));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader){

    std::unordered_map<std::string, unsigned int> num_st{
                {LightTypeStr(LightType::DIFFUSE), 1}, 
                {LightTypeStr(LightType::SPECULAR), 1}, 
                {LightTypeStr(LightType::NORMAL), 1}, 
                {LightTypeStr(LightType::HEIGHT), 1}};

    for(size_t i=0; i < textures_.size(); i++){
        unsigned int& index = num_st.at(textures_[i].type);
        glActiveTexture(GL_TEXTURE0 + i);
        std::string smapler_name = "tex_" + textures_[i].type + std::to_string(index);
        glUniform1i(glGetUniformLocation(shader.shader_program_, smapler_name.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        index++;
    }

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



class Model{
public:
    Model(const std::string& model_path);

    void load_model(const std::string& model_path);

    void process_node(const aiNode* node, const aiScene* scene);

    Mesh process_mesh(const aiMesh* ai_mesh, const aiScene* scene);

    std::vector<Texture> process_material(const aiMaterial* material, aiTextureType material_type);

    // - 
    void setup_mesh();

    void draw(Shader& shader);

public:
    std::string directory_;
    std::unordered_map<std::string, Texture> loaded_texture;

    std::vector<Mesh> meshes_;

};


Model::Model(const std::string& model_path){
    Assimp::Importer importer;
    std::string obj_path = model_path;
    const aiScene* scene = importer.ReadFile(obj_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << " - import fail, path = " << obj_path << std::endl; 
        exit(-1);
    }

    directory_ = model_path.substr(0, model_path.find_last_of("/"));
    std::cout << " - directory_ " << directory_ << std::endl;

    process_node(scene->mRootNode, scene);

    std::cout << "OUT: mesh count " << meshes_.size() << std::endl;
    std::cout << "OUT: textures sum " << loaded_texture.size() << std::endl;
    for(size_t i=0; i<meshes_.size(); i++){
        std::cout << " --- id " << i << std::endl;
        std::cout << " vertices " << meshes_[i].vertices_.size() << std::endl;
        std::cout << " inddices " << meshes_[i].indices_.size() / 3.0 << std::endl;
        std::cout << " textures_ " << meshes_[i].textures_.size() << std::endl;
    }
}

void Model::setup_mesh(){
    for(size_t i=0; i< meshes_.size(); i++){
        meshes_[i].setup_mesh();
    }
}

void Model::draw(Shader& shader){
    for(size_t i=0; i<meshes_.size(); i++){
        meshes_[i].draw(shader);
    }
}


void Model::process_node(const aiNode* node, const aiScene* scene){
    // - process multi meshs
    for(unsigned int i=0; i<node->mNumMeshes; i++){
        Mesh a_mesh = process_mesh(scene->mMeshes[node->mMeshes[i]], scene);
        meshes_.emplace_back(std::move(a_mesh));
    }

    // - process children
    for(unsigned int j = 0; j<node->mNumChildren; j++){
        process_node(node->mChildren[j], scene);
    }
}


Mesh Model::process_mesh(const aiMesh* ai_mesh, const aiScene* scene){
    // - postion
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<unsigned int> indices;

    if (!ai_mesh->HasPositions()){
        std::cout << "WARN: no postions." << std::endl;
        return Mesh();
    }
    if(!ai_mesh ->HasFaces()){
        std::cout << "WARN: no faces." << std::endl;
        return Mesh();
    }

    for(unsigned int i=0; i < ai_mesh->mNumVertices; i++){
        Vertex a_vertex;
        a_vertex.pos.x = ai_mesh->mVertices[i].x;
        a_vertex.pos.y = ai_mesh->mVertices[i].y;
        a_vertex.pos.z = ai_mesh->mVertices[i].z;

        if(ai_mesh->HasNormals()){
            a_vertex.normal.x = ai_mesh->mNormals[i].x;
            a_vertex.normal.y = ai_mesh->mNormals[i].y;
            a_vertex.normal.z = ai_mesh->mNormals[i].z;
        }


        if(ai_mesh->mTextureCoords[0]){
            a_vertex.tex_coord.x = ai_mesh->mTextureCoords[0][i].x;
            a_vertex.tex_coord.y = ai_mesh->mTextureCoords[0][i].y;

            a_vertex.tangent.x = ai_mesh->mTangents[i].x;
            a_vertex.tangent.y = ai_mesh->mTangents[i].y;
            a_vertex.tangent.z = ai_mesh->mTangents[i].z;

            a_vertex.bitangent.x = ai_mesh->mBitangents[i].x;
            a_vertex.bitangent.y = ai_mesh->mBitangents[i].y;
            a_vertex.bitangent.z = ai_mesh->mBitangents[i].z;
        }

        vertices.emplace_back(a_vertex);
    }

    for(size_t i=0; i < ai_mesh->mNumFaces; i++){
        const aiFace face = ai_mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];

    std::vector<Texture> tex_diffuse = process_material(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), tex_diffuse.begin(), tex_diffuse.end());

    std::vector<Texture> tex_specular = process_material(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), tex_specular.begin(), tex_specular.end());

    std::vector<Texture> tex_normal = process_material(material, aiTextureType_NORMALS);
    textures.insert(textures.end(), tex_normal.begin(), tex_normal.end());

    std::vector<Texture> tex_height = process_material(material, aiTextureType_HEIGHT);
    textures.insert(textures.end(), tex_height.begin(), tex_height.end());
    
    Mesh out_mesh;
    out_mesh.vertices_ = std::move(vertices);
    out_mesh.indices_ = std::move(indices);
    out_mesh.textures_ = std::move(textures);

    return out_mesh;
}

std::vector<Texture> Model::process_material(const aiMaterial* material, aiTextureType tex_type){
    
    std::vector<Texture> textures;
    std::unordered_map<aiTextureType, std::string> type_name_map{
                            {aiTextureType_DIFFUSE, LightTypeStr(LightType::DIFFUSE)}, 
                            {aiTextureType_SPECULAR, LightTypeStr(LightType::SPECULAR)},
                            {aiTextureType_HEIGHT, LightTypeStr(LightType::HEIGHT)},
                            {aiTextureType_NORMALS, LightTypeStr(LightType::NORMAL)}};

    size_t count = material->GetTextureCount(tex_type);
    if(count == 0)
        return textures;
    
    textures.reserve(count);

    for(size_t i=0; i<count; i++){
        aiString name;
        material->GetTexture(tex_type, i, &name);
        if(loaded_texture.find(name.C_Str()) == loaded_texture.end()){
            const std::string img_path = directory_ + "/" + name.C_Str();
            unsigned int tex_id = load_texture(img_path);
            Texture texture(tex_id, type_name_map[tex_type], name.C_Str());
            textures.emplace_back(texture);
            
            loaded_texture.insert({name.C_Str(), texture});
        }else{
            textures.emplace_back(loaded_texture[name.C_Str()]);
        }
        
    }
    return textures;
}


#endif