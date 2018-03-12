//
// Created by jonas on 7/25/15.
//

#include "GBuffer.h"
#include "GLError.h"

#include <gtx/transform.hpp>
#include <geometric.hpp>
#include <gtc/type_ptr.hpp>
#include <vec2.hpp>
#include <gtc/random.hpp>
#include <iostream>
#include <vector>
#include "MyUtility.h"

#define SAMPLE_RADIUS 300.0f
#define SAMPLES 19

GBuffer::GBuffer(int width, int height) :
gen_shader("../shaders/gen_gbuffer.vert","../shaders/gen_gbuffer.frag","../shaders/gen_gbuffer.geom")
{
    this->width = width;
    this->height = height;

    check_gl_error();

    setupTextures();

    check_gl_error();
}

GBuffer::GBuffer() {

}

void GBuffer::generate(std::vector<MeshObject>* meshes, Camera* cam) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    currentState = !currentState;

    GLuint activeDepth = currentState ? depths : depths2;
    GLuint depthToPass = currentState ? depths2 : depths;

    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,activeDepth,0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,normals,0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,diffColors,0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,specColors,0);

    GLuint buffers[3] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};

    glDrawBuffers(3,buffers);

    glClearColor(0,0,0,0);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    gen_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthToPass);
    glUniform1i(gen_shader.get_U_Location(Shader::U_I_DEPTH_TEX),0);

    glm::mat4 model = glm::mat4();
    glm::mat4 view = cam->getLookAt();
    glm::mat4 projection = cam->getProjection();

    glm::mat4 vm = view * model;
    glm::mat4 pvm = projection * vm;

    glUniformMatrix4fv(gen_shader.get_U_Location(Shader::U_M_VM),1,GL_FALSE,glm::value_ptr(vm));

    glUniformMatrix4fv(gen_shader.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

    glUniform1f(gen_shader.get_U_Location(Shader::U_F_FARPLANE),cam->farPlane);
    glUniform1f(gen_shader.get_U_Location(Shader::U_F_NEARPLANE),cam->nearPlane);

    for(std::vector<MeshObject>::iterator iter = meshes->begin(); iter != meshes->end();iter++) {
        glBindVertexArray(iter->getVAO());

        glUniform4fv(gen_shader.get_U_Location(Shader::U_V4_DIFF_COL),1,glm::value_ptr(iter->diffColor));
        glUniform4fv(gen_shader.get_U_Location(Shader::U_V4_SPEC_COL),1,glm::value_ptr(iter->specColor));

        glDrawElements(GL_TRIANGLES,iter->getNoIndices(),GL_UNSIGNED_INT,(void*) 0);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    //glDepthRangef(0,1);

    check_gl_error();
}

GLuint GBuffer::getBuffer(renderTarget render) {
    switch(render) {
        case e_depths:
            return currentState ? depths : depths2;
        case e_diffColors:
            return diffColors;
        case e_specColors:
            return specColors;
        case e_normals:
            return normals;
        default:
            std::cout << "Invalid buffer requested" << std::endl;
    }
}

void GBuffer::setupTextures() {
    glGenFramebuffers(1,&FBO);

    glGenTextures(1, &normals);
    glBindTexture(GL_TEXTURE_2D_ARRAY,normals);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGB16F,width,height,2,0,GL_RGB,GL_FLOAT,0);

    std::cout << "normals: " << normals << std::endl;

    glGenTextures(1, &diffColors);
    glBindTexture(GL_TEXTURE_2D_ARRAY,diffColors);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGBA8,width,height,2,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

    glGenTextures(1, &specColors);
    glBindTexture(GL_TEXTURE_2D_ARRAY,specColors);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGBA8,width,height,2,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

    glGenTextures(1, &depths);
    glBindTexture(GL_TEXTURE_2D_ARRAY,depths);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_DEPTH_COMPONENT32F,width,height,2,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);

    glGenTextures(1, &depths2);
    glBindTexture(GL_TEXTURE_2D_ARRAY,depths2);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_DEPTH_COMPONENT32F,width,height,2,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);

    check_gl_error();

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindTexture(GL_TEXTURE_2D,0);

    check_gl_error();
}

void GBuffer::setBufferSizes(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    glDeleteTextures(1,&normals);
    glDeleteTextures(1,&specColors);
    glDeleteTextures(1,&diffColors);
    glDeleteTextures(1,&depths);
    glDeleteTextures(1,&depths2);

    glDeleteFramebuffers(1,&FBO);

    setupTextures();
}