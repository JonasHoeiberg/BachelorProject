//
// Created by Jonas on 25-11-2015.
//

#ifndef BSCPROJECT_FILTERS_H
#define BSCPROJECT_FILTERS_H

#include <map>
#include "GL/glew.h"
#include "Camera.h"
#include "shader.h"
#include "GBuffer.h"
#include "light.h"
/*
enum textureTarget {
    radiosity_1,
    radiosity_2,
    ssao,
    glossy
};
*/

class Filter {
public:
    Filter(const std::string& pathToVert, const std::string& pathToFrag, const std::string& pathToGeom);
    virtual void updateSize(unsigned int width, unsigned int height);
    void setFBO(GLuint FBO);
protected:
    Shader::ShaderProgram program;
    GLuint FBO;
    unsigned int width, height;
};

class GaussFilterOneCh : public Filter {
public:
    GaussFilterOneCh(unsigned int width, unsigned int height, GLuint dstTex);
    void applyGauss(GBuffer *buffer, GLuint srcTex);
private:
    Shader::ShaderProgram vert_shader;
    GLuint secondPassFBO, tempTex, dstTex;
};

class GaussFilterWBlend : public Filter {
public:
    GaussFilterWBlend(unsigned int width, unsigned int height, GLuint dstTex, GLuint totalTex, GLuint edges);
    void applyGauss(GBuffer *buffer, GLuint srcTex);
private:
    Shader::ShaderProgram vert_shader;
    GLuint secondPassFBO, tempTex, dstTex, totalTex, edgeTex;
};

class LambertianFilter : public Filter {
public:
    LambertianFilter(unsigned int width, unsigned int height, GLuint nextTarget, GLuint totTarget);
    void setTargets(GLuint target, GLuint totTarget);
    void applyLambertian(Camera *cam, GBuffer *buffer, std::vector<Light> &lights);
};

class RadiosityFilter : public Filter {
public:
    RadiosityFilter(unsigned int width, unsigned int height, GLuint prevBounce, GLuint totalRad, GLuint edges);
    void applyBounce(Camera *cam, GBuffer *buffer);
    //The nextbounce texture returns with the filtered result of the bounce,
    //the totalrad texture returns with the total radiosity since Lambertian was last used
    virtual void updateSize(unsigned int height, unsigned int width);

    GLuint getTempRadTex() const {
        return tempRadTex;
    }
private:
    GLuint randTex = -1;
    GLuint tempRadTex = -1;
    GLuint prevBounce = -1;
    GaussFilterWBlend gauss;
};

class AOFilter : public Filter {
public:
	AOFilter(unsigned int width, unsigned int height, GLuint AOTarget);
	void applySSAO(Camera *cam, GBuffer *buffer);

	GLuint getTempAOTex() const {
		return tempAOTex;
	}
private:
	GLuint randTex = -1;
	GLuint tempAOTex = -1;
	GaussFilterOneCh gauss;
};

class edgeDetectionFilter : public Filter {
public:
	edgeDetectionFilter(unsigned int width, unsigned int height, GLuint target);
	void genEdgeMap(Camera *cam, GBuffer *buffer);
};

#endif //BSCPROJECT_FILTERS_H
