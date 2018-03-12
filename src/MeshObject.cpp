//
// Created by Jonas on 14-03-2015.
//

#include <gtx/transform.hpp>
#include <geometric.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <functional>
#include <gtc/constants.hpp>

#include "MeshObject.h"


glm::mat4x4 MeshObject::getModel() {
    return glm::translate(translation) * glm::scale(scale) * glm::mat4_cast(rotation);
}

GLuint MeshObject::getVAO() {
    return vao;
}

MeshObject::MeshObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::string& texturePath) :
isColored(false)
{

    rotation = glm::fquat(0,0,0,0);
    translation = glm::vec3(0);
    scale = glm::vec3(1.0f);

    setupVAO(vertices, indices);

}

void MeshObject::setupVAO(std::vector<Vertex> const &vertices, std::vector<GLuint> const &indices) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));

    GLuint ibuffer;
    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_STATIC_DRAW);

    glBindVertexArray(0);

    noIndices = indices.size();

    std::cout << "A new mesh was added with " << noIndices << " indices" << std::endl;
}



MeshObject::MeshObject()
{

}

GLuint MeshObject::getNoIndices() {
    return noIndices;
}

void MeshObject::scaleBy(const glm::vec3 &scaleBy) {
    scale *= scaleBy;
}

void MeshObject::translate(const glm::vec3 &trans) {
    translation += trans;
}

MeshObject::~MeshObject() {
}

void MeshObject::clean() {
    glDeleteVertexArrays(1,&vao);
}

MeshObject::MeshObject(aiMesh *mesh,aiMaterial *material) :
isColored(true)
{
    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    for(int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(Vertex(
                glm::vec3(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z),
                glm::vec3(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z),
                glm::vec2(0,0)
        ));
    }

    for(int i = 0; i < mesh->mNumFaces; i++) {
        aiFace currentFace = mesh->mFaces[i];
        for(int j = 0; j < currentFace.mNumIndices; j++) {
            indices.push_back(currentFace.mIndices[j]);
        }
    }

    aiColor4D diff;
    aiColor4D spec;
    float shininess;

    material->Get(AI_MATKEY_COLOR_DIFFUSE,diff);
    material->Get(AI_MATKEY_COLOR_SPECULAR,spec);
    material->Get(AI_MATKEY_SHININESS,shininess);

    shininess /= 4.0f;

    diffColor = glm::vec4(diff.r,diff.g,diff.b,diff.a);
    specColor = glm::vec4(spec.r,spec.g,spec.b,shininess);

    setupVAO(vertices,indices);

    switch(mesh->mPrimitiveTypes) {
        case (aiPrimitiveType_TRIANGLE) :
            type = GL_TRIANGLES;
            break;
        case(aiPrimitiveType_LINE) :
            type = GL_LINES;
            break;
        default:
            std::cout << "You're not supposed to see this" << std::endl;
            break;
    }
}
