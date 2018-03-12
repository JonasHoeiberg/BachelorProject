//
// Created by Jonas on 14-03-2015.
//

#ifndef _GLFWTEST_MESHOBJECT_H_
#define _GLFWTEST_MESHOBJECT_H_

#include <GL/glew.h>
#include <vector>
#include <string>
#include <vec3.hpp>
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/quaternion.hpp>
#include <assimp/scene.h>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 UV;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 UV) {
        this->position = position;
        this->normal = normal;
        this->UV = UV;
    }
};

class MeshObject {
public:
    MeshObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices,
               const std::string& texturePath = std::string());
    MeshObject(aiMesh* assimpMesh, aiMaterial* material);
    MeshObject();
    ~MeshObject();

    GLuint getVAO();
    GLuint getNoIndices();

    glm::mat4x4 getModel();

    void translate(const glm::vec3& trans);
    void scaleBy(const glm::vec3& scaleBy);

    void clean();

    bool isColored;
    glm::vec4 diffColor;
    glm::vec4 specColor;

private:
    GLuint vao;

    GLuint noIndices;
    GLuint texture = 0;

    GLenum type;

    glm::fquat rotation;
    glm::vec3 translation;
    glm::vec3 scale;

    void setupVAO(std::vector<Vertex> const &vertices, std::vector<GLuint> const &indices);
};


#endif //_GLFWTEST_MESHOBJECT_H_
