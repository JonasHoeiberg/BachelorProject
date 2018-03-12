//
// Created by jonas on 7/25/15.
//

#ifndef BSCPROJECT_GBUFFER_H
#define BSCPROJECT_GBUFFER_H

#include <GL/glew.h>
#include <string>
#include "shader.h"
#include "MeshObject.h"
#include "Camera.h"
#include <vector>

enum renderTarget {
    e_specColors,
    e_diffColors,
    e_normals,
    e_depths
};


class GBuffer {
public:
    GBuffer(int width, int height);
    GBuffer();

    void generate(std::vector<MeshObject>* meshes, Camera* cam);
    //Generates the layered G-buffer based on the meshes supplied. Projection and view will have to be set before.
    GLuint getBuffer(renderTarget render);

    void setupTextures();
    //Returns a texture as requested and the sets the refered int to the number of layers
    //in the texture. Mostly used for debugging
    //Sets a property of the rendering (mostly shader #defines)

    void setBufferSizes(unsigned int width, unsigned int height);
private:
    //Texture references
    GLuint normals, depths, depths2, diffColors, specColors;
    //Frame buffer objects
    GLuint FBO;

    int width, height;

    //currentState specifies which texture to use for layer 1 depth comparison
    //and where prev layer radiosity is.
    bool currentState = true;

    Shader::ShaderProgram gen_shader;
};


#endif //BSCPROJECT_GBUFFER_H
