//
// Created by Jonas on 25-11-2015.
//

#include "filters.h"
#include "MyUtility.h"
#include "GLError.h"
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>
#include <iostream>

Filter::Filter(const std::string& pathToVert, const std::string& pathToFrag, const std::string& pathToGeom) :
program(pathToVert,pathToFrag,pathToGeom)
{
	glGenFramebuffers(1,&FBO);
}
void Filter::updateSize(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
}

void Filter::setFBO(GLuint FBO) {
	this->FBO = FBO;
}

LambertianFilter::LambertianFilter(unsigned int width, unsigned int height, GLuint nextTarget, GLuint totTarget) :
Filter("../shaders/texture_quad_w_geom.vert","../shaders/lambert.frag","../shaders/layered_quad.geom")
{
	Filter::updateSize(width,height);

	setTargets(nextTarget,totTarget);
}

void LambertianFilter::setTargets(GLuint target, GLuint totTarget) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, target,0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, totTarget,0);

	GLuint buffers[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};

	glDrawBuffers(2,buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void LambertianFilter::applyLambertian(Camera *cam, GBuffer *buffer, std::vector<Light> &lights) {

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glDisable(GL_DEPTH_TEST);

	program.use();

	glBindVertexArray(FSQuad::getVAO());

	glClearColor(0,0,0,1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 pvm = FSQuad::getPV();

    glm::mat4 invProjection = glm::inverse(cam->getProjection());

    glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

    glUniformMatrix4fv(program.get_U_Location(Shader::U_M_VUNPROJECT),1,GL_FALSE,
                       glm::value_ptr(invProjection));

    glUniform1f(program.get_U_Location(Shader::U_F_FARPLANE),cam->farPlane);

    glUniformMatrix4fv(program.get_U_Location(Shader::U_M_UNVIEW),1,GL_FALSE,glm::value_ptr(glm::inverse(cam->getLookAt())));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_normals));
	glUniform1i(program.get_U_Location(Shader::U_I_NORM_TEX),1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_diffColors));
	glUniform1i(program.get_U_Location(Shader::U_I_DIFF_TEX),2);

	check_gl_error();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

    for(int i = 0; i < lights.size(); i++) {
        glm::vec3 lightPos = lights[i].getPosition();
        glm::vec3 lightIntensity = lights[i].getIntensity();

        glUniform3fv(program.get_U_Location(Shader::U_V3_LIGHTPOS), 1, glm::value_ptr(lightPos));

        glUniform3fv(program.get_U_Location(Shader::U_V3_LIGHT_INTENSITY), 1, glm::value_ptr(lightIntensity));

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP,lights[i].getShadowMap());
        glUniform1i(program.get_U_Location(Shader::U_I_SHADOWMAP),3);

        check_gl_error();

        glDrawElements(GL_TRIANGLES, FSQuad::getNoIndices(), GL_UNSIGNED_INT, (void *) 0);
    }

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	check_gl_error();
}

RadiosityFilter::RadiosityFilter(unsigned int width, unsigned int height, GLuint prevBounce, GLuint totalRad, GLuint edges)
		:
Filter("../shaders/texture_quad_w_geom.vert","../shaders/apply_rad_bounce.frag","../shaders/layered_quad.geom"),
gauss(width, height, prevBounce, totalRad, edges) {

	updateSize(width,height);

	this->prevBounce = prevBounce;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tempRadTex,0);

	GLuint buffers = GL_COLOR_ATTACHMENT0;

	glDrawBuffers(1,&buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RadiosityFilter::applyBounce(Camera *cam, GBuffer *buffer) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glDisable(GL_DEPTH_TEST);

	program.use();

	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glm::mat4 invProjection = glm::inverse(cam->getProjection());

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_VUNPROJECT),1,GL_FALSE,
					   glm::value_ptr(invProjection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_normals));
	glUniform1i(program.get_U_Location(Shader::U_I_NORM_TEX),1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_diffColors));
	glUniform1i(program.get_U_Location(Shader::U_I_DIFF_TEX),2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY,prevBounce);
	glUniform1i(program.get_U_Location(Shader::U_I_PREVRAD),3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,randTex);
	glUniform1i(program.get_U_Location(Shader::U_I_NOISE_TEX),4);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	glEnable(GL_DEPTH_TEST);

	gauss.applyGauss(buffer, tempRadTex);
}

void RadiosityFilter::updateSize(unsigned int width, unsigned int height) {
	Filter::updateSize(width, height);

	gauss.updateSize(width,height);

	glDeleteTextures(1, &randTex);
	glDeleteTextures(1, &tempRadTex);

	std::vector<float> randNoise = std::vector<float>(width*height);

	for(unsigned long int i = 0; i < width*height; i++) {

		float angle = glm::linearRand<float>(0.0f, 2.0f * glm::pi<float>());
		randNoise[i] = angle;//glm::vec2(glm::cos(angle),glm::sin(angle));
	}

	glGenTextures(1, &randTex);
	glBindTexture(GL_TEXTURE_2D,randTex);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,width,height,0,GL_RED,GL_FLOAT,&(randNoise[0]));

	glGenTextures(1, &tempRadTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY,tempRadTex);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R11F_G11F_B10F,width,height,2,0,GL_RGB,GL_FLOAT,0);

	glBindTexture(GL_TEXTURE_2D,0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);

	check_gl_error();
}

GaussFilterWBlend::GaussFilterWBlend(unsigned int width, unsigned int height, GLuint dstTex, GLuint totalTex, GLuint edges)
		:
Filter("../shaders/texture_quad_w_geom.vert","../shaders/bilat_hor.frag","../shaders/layered_quad.geom"),
vert_shader("../shaders/texture_quad_w_geom.vert","../shaders/bilat_vert_wblend.frag","../shaders/layered_quad.geom"){
	Filter::updateSize(width,height);

	this->edgeTex = edges;

	glGenFramebuffers(1,&secondPassFBO);

	glGenTextures(1, &tempTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY,tempTex);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R11F_G11F_B10F,width,height,2,0,GL_RGB,GL_FLOAT,0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tempTex,0);

	GLuint buffers = GL_COLOR_ATTACHMENT0;

	glDrawBuffers(1,&buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,secondPassFBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, dstTex,0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, totalTex,0);

	GLuint buffersArray[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};

	glDrawBuffers(2,buffersArray);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void GaussFilterWBlend::applyGauss(GBuffer *buffer, GLuint srcTex) {
	glDisable(GL_DEPTH_TEST);

	//Horizontal pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	program.use();
	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,srcTex);
	glUniform1i(program.get_U_Location(Shader::U_I_PREVRAD),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY,edgeTex);
	glUniform1i(program.get_U_Location(Shader::U_I_EDGE_TEX),2);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	//Vertical pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,secondPassFBO);

	glEnablei(GL_BLEND,1);
	glBlendFunc(GL_ONE,GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	vert_shader.use();
	glBindVertexArray(FSQuad::getVAO());

	glUniformMatrix4fv(vert_shader.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,tempTex);
	glUniform1i(vert_shader.get_U_Location(Shader::U_I_PREVRAD),0);

	glUniform1i(vert_shader.get_U_Location(Shader::U_I_DEPTH_TEX),1);

	glUniform1i(vert_shader.get_U_Location(Shader::U_I_EDGE_TEX),2);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	glDisablei(GL_BLEND,1);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	glBindTexture(GL_TEXTURE_2D,0);

	glEnable(GL_DEPTH_TEST);
}

GaussFilterOneCh::GaussFilterOneCh(unsigned int width, unsigned int height, GLuint dstTex):
		Filter("../shaders/texture_quad.vert","../shaders/bilat_1ch_hor.frag",""),
		vert_shader("../shaders/texture_quad.vert","../shaders/bilat_1ch_vert.frag") {
	Filter::updateSize(width,height);

	glGenFramebuffers(1,&secondPassFBO);

	glGenTextures(1, &tempTex);
	glBindTexture(GL_TEXTURE_2D,tempTex);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,width,height,0,GL_RED,GL_FLOAT,0);
	glBindTexture(GL_TEXTURE_2D,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tempTex,0);

	GLuint buffers = GL_COLOR_ATTACHMENT0;

	glDrawBuffers(1,&buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,secondPassFBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, dstTex,0);

	glDrawBuffers(1,&buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void GaussFilterOneCh::applyGauss(GBuffer *buffer, GLuint srcTex) {
	glDisable(GL_DEPTH_TEST);

	//Horizontal pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	program.use();
	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,srcTex);
	glUniform1i(program.get_U_Location(Shader::U_I_TEXTURE),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),1);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	//Vertical pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,secondPassFBO);

	vert_shader.use();
	glBindVertexArray(FSQuad::getVAO());

	glUniformMatrix4fv(vert_shader.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tempTex);
	glUniform1i(vert_shader.get_U_Location(Shader::U_I_TEXTURE),0);

	glUniform1i(vert_shader.get_U_Location(Shader::U_I_DEPTH_TEX),1);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	glBindTexture(GL_TEXTURE_2D,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	glEnable(GL_DEPTH_TEST);

	check_gl_error();
}

AOFilter::AOFilter(unsigned int width, unsigned int height, GLuint AOTarget) :
		Filter("../shaders/texture_quad.vert","../shaders/apply_SSAO.frag",""),
		gauss(width, height, AOTarget) {

	updateSize(width,height);

	std::vector<float> randNoise = std::vector<float>(width*height);

	for(unsigned long int i = 0; i < width*height; i++) {

		float angle = glm::linearRand<float>(0.0f, 2.0f * glm::pi<float>());
		randNoise[i] = angle;
	}

	glGenTextures(1, &randTex);
	glBindTexture(GL_TEXTURE_2D,randTex);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,width,height,0,GL_RED,GL_FLOAT,&(randNoise[0]));

	glGenTextures(1, &tempAOTex);
	glBindTexture(GL_TEXTURE_2D,tempAOTex);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,width,height,0,GL_RED,GL_FLOAT,0);

	glBindTexture(GL_TEXTURE_2D,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tempAOTex,0);

	GLuint buffers = GL_COLOR_ATTACHMENT0;

	glDrawBuffers(1,&buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	check_gl_error();
}

void AOFilter::applySSAO(Camera *cam, GBuffer *buffer) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glDisable(GL_DEPTH_TEST);

	program.use();

	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glm::mat4 invProjection = glm::inverse(cam->getProjection());

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_VUNPROJECT),1,GL_FALSE,
	                   glm::value_ptr(invProjection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_normals));
	glUniform1i(program.get_U_Location(Shader::U_I_NORM_TEX),1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,randTex);
	glUniform1i(program.get_U_Location(Shader::U_I_NOISE_TEX),2);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	glBindTexture(GL_TEXTURE_2D,0);

	glEnable(GL_DEPTH_TEST);

	check_gl_error();

	gauss.applyGauss(buffer, tempAOTex);
}

edgeDetectionFilter::edgeDetectionFilter(unsigned int width, unsigned int height, GLuint target) :
Filter("../shaders/texture_quad_w_geom.vert","../shaders/detect_edges.frag","../shaders/layered_quad.geom"){
	Filter::updateSize(width,height);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, target,0);

	GLuint buffers[1] = {GL_COLOR_ATTACHMENT0};

	glDrawBuffers(1,buffers);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void edgeDetectionFilter::genEdgeMap(Camera *cam, GBuffer *buffer) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glDisable(GL_DEPTH_TEST);

	program.use();

	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),1,GL_FALSE,glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, buffer->getBuffer(e_depths));
	glUniform1i(program.get_U_Location(Shader::U_I_DEPTH_TEX),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY,buffer->getBuffer(e_normals));
	glUniform1i(program.get_U_Location(Shader::U_I_NORM_TEX),1);

	glUniform1f(program.get_U_Location(Shader::U_F_NEARPLANE),cam->nearPlane);

	glUniform1f(program.get_U_Location(Shader::U_F_FARPLANE),cam->farPlane);

	glDrawElements(GL_TRIANGLES,FSQuad::getNoIndices(),GL_UNSIGNED_INT,(void*) 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	glBindTexture(GL_TEXTURE_2D,0);

	glEnable(GL_DEPTH_TEST);
}
