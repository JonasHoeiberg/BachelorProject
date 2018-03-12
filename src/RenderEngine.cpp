//
// Created by jonas on 7/3/15.
//

#include "RenderEngine.h"
#include "shader.h"
#include "GLError.h"
#include "GBuffer.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

RenderEngine::RenderEngine(std::vector<Message *> *messages, GLFWwindow *window) :
		buffer(width,height),
		dbg(width,height),
		cam(glm::vec3(400,350,-300),glm::normalize(glm::vec3(-1,0,2))),
		time("Generation Time")
{
	prevTime = glfwGetTime();

	this->messages = messages;
	context = window;

	isLayered = false;

	check_gl_error();
}

bool RenderEngine::initEngine(int width, int height) {

	this->width = width;
	this->height = height;

	glfwSetInputMode(context,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	glViewport(0,0,width,height);
	glClearColor(0,0,0,1);

	updateBuffers(context,width,height);

	parseImportFile("res/models/conference/conference.obj");

	lights.push_back(Light(glm::vec3(0.0f,400.0f,0.0f),glm::vec3(4.0f,4.0f,4.0f)));

	dbg.addTimer(time.getName(),time.getAvgTimePtr());
	time.activate();

	initTextures(width,height);

	FSQuad::setupQuad();

	check_gl_error();
}

void RenderEngine::draw() {

	dt = glfwGetTime() - prevTime;
	prevTime = glfwGetTime();

	static LambertianFilter lambert = LambertianFilter(width,height,radPrev,radTot);
	static RadiosityFilter radFilter = RadiosityFilter(width, height, radPrev, radTot, edges);
	static AOFilter ssaoFilter = AOFilter(width,height,alchAO);
	static edgeDetectionFilter edgeFilter = edgeDetectionFilter(width,height,edges);

	parseRenderMessages();

	cam.update();

	buffer.generate(&meshes, &cam);

	for(int i = 0; i < lights.size(); i++) {
		lights[i].genShadowMap(&cam,&meshes);
	}

	//if(animate)
	//    lights[0].animate(dt);

	glViewport(0,0,width,height);

	edgeFilter.genEdgeMap(&cam,&buffer);

	lambert.applyLambertian(&cam, &buffer, lights);

	time.startTimer();
	for(int i = 0; i < bounces; i++) {
		radFilter.applyBounce(&cam,&buffer);
	}
	time.endTimer();

	ssaoFilter.applySSAO(&cam,&buffer);

	glClearColor(0,1,0,1);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//Render to backbuffer.
	if(backBufferTexture != -1)
		renderTextureLevel(backBufferTexture,backBufferLayer);
	else
		renderComposite();

	dbg.draw();

	glfwSwapBuffers(context);

	check_gl_error();
}

void RenderEngine::renderTextureLevel(GLuint texture, unsigned short int layer) {
	static Shader::ShaderProgram tex_shader = Shader::ShaderProgram("../shaders/texture_quad.vert",
																	"../shaders/layered_texture.frag");

	static Shader::ShaderProgram nolayers_tex_shader = Shader::ShaderProgram("../shaders/texture_quad.vert",
	                                                                         "../shaders/unlayered_texture.frag");

	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	if(isLayered) {

		tex_shader.use();

		glUniformMatrix4fv(tex_shader.get_U_Location(Shader::U_M_PVM), 1, GL_FALSE, glm::value_ptr(pvm));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		glUniform1i(tex_shader.get_U_Location(Shader::U_I_TEXTURE), 0);

		glUniform1i(tex_shader.get_U_Location(Shader::U_I_LAYER), layer);

		glDrawElements(GL_TRIANGLES, FSQuad::getNoIndices(), GL_UNSIGNED_INT, (void *) 0);
	} else {

		nolayers_tex_shader.use();

		glUniformMatrix4fv(nolayers_tex_shader.get_U_Location(Shader::U_M_PVM), 1, GL_FALSE, glm::value_ptr(pvm));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(nolayers_tex_shader.get_U_Location(Shader::U_I_TEXTURE), 0);

		glDrawElements(GL_TRIANGLES, FSQuad::getNoIndices(), GL_UNSIGNED_INT, (void *) 0);
	}
}

void RenderEngine::renderComposite() {
	static Shader::ShaderProgram final_shader = Shader::ShaderProgram("../shaders/texture_quad.vert",
	                                                                "../shaders/compose.frag");

	final_shader.use();

	glBindVertexArray(FSQuad::getVAO());

	glm::mat4 pvm = FSQuad::getPV();

	glUniformMatrix4fv(final_shader.get_U_Location(Shader::U_M_PVM), 1, GL_FALSE, glm::value_ptr(pvm));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, alchAO);
	glUniform1i(final_shader.get_U_Location(Shader::U_I_AO), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, buffer.getBuffer(e_diffColors));
	glUniform1i(final_shader.get_U_Location(Shader::U_I_DIFF_TEX), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, radTot);
	glUniform1i(final_shader.get_U_Location(Shader::U_I_RAD), 2);

	glDrawElements(GL_TRIANGLES, FSQuad::getNoIndices(), GL_UNSIGNED_INT, (void *) 0);
}

void RenderEngine::parseRenderMessages() {
	for(std::vector<Message*>::iterator iter = messages->begin(); iter != messages->end(); iter++) {
		if(*iter == nullptr)
			continue;

		Message* msg = *iter;

		switch(msg->type) {
			case CLEAR_COLOR_MSG : {    //Not much use for this. Used to test messaging system and developer console
				glm::vec3& color = msg->extractMsgData<glm::vec3>();
				glClearColor(color.x, color.y, color.z, 1);
				msg->kill = true;
				break;
			}
			case CONSOLE_TOGGLE_MSG : { //This is being handled here because the debug drawer needs to know when the console is toggled
				std::string* text = msg->extractMsgData<std::string*>();
				dbg.toggleConsole(text);
				cam.lock = !cam.lock;
				if(!cam.lock)
					glfwSetInputMode(context,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
				else
					glfwSetInputMode(context,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
				msg->kill = true;
				break;
			}
			case CAM_DESIGNATION_MSG : {
				ExposeControlButton& info = msg->extractMsgData<ExposeControlButton>();
				cam.setDesignation(info.dir,info.button);
				msg->kill = true;
				break;
			}
			case CAM_EXPOSE_MOUSE : {
				glm::vec2* mouseInfo = msg->extractMsgData<glm::vec2*>();
				cam.setRotationControl(mouseInfo);
				msg->kill = true;
				break;
			}
			case CAM_SPEED_MSG : { ;
				cam.setSpeed(msg->extractMsgData<float>());
				msg->kill = true;
				break;
			}
			case DRAW_TEXTURE_MSG : {
				std::string& textureString = msg->extractMsgData<std::string>();
				if(textureString == "depths") {
					backBufferTexture = buffer.getBuffer(e_depths);
					isLayered = true;
				}
				if(textureString == "diffcolor") {
					backBufferTexture = buffer.getBuffer(e_diffColors);
					isLayered = true;
				}
				else if(textureString == "speccolor") {
					backBufferTexture = buffer.getBuffer(e_specColors);
					isLayered = true;
				}
				else if(textureString == "normals") {
					backBufferTexture = buffer.getBuffer(e_normals);
					isLayered = true;
				}
				else if(textureString == "totalrad") {
					backBufferTexture = radTot;
					isLayered = true;
				}
				else if(textureString == "prevrad") {
					backBufferTexture = radPrev;
					isLayered = true;
				}
					/*
				else if(textureString == "nextrad") {
					backBufferTexture = radNext;
					isLayered = true;
				}
					 */
				else if(textureString == "ssao") {
					backBufferTexture = alchAO;
					isLayered = false;
				}
				else if(textureString == "edges") {
					backBufferTexture = edges;
					isLayered = true;
				}
				else if(textureString == "final") {
					backBufferTexture = -1;
					isLayered = false;
				}
				msg->kill = true;
				break;
			}
			case DRAW_LAYER_MSG : {
				float& layer = msg->extractMsgData<float>();
				if(layer < 2)
					backBufferLayer = (unsigned short int) layer;
				msg->kill = true;
				break;
			}
			case TOGGLE_TEST_MSG: {
				doTest = !doTest;
				bounces = doTest ? -1 : 0;
				msg->kill = true;
				break;
			}
			case TEST_BOUNCE_MSG: {
				bounces = msg->extractMsgData<int>();
				msg->kill = true;
				break;
			}
			case LIGHT_POS_MSG: {
				lights[0].setPosition(msg->extractMsgData<glm::vec3>());
				msg->kill = true;
				break;
			}
			case STOP_ANIM_MSG: {
				animate = !animate;
				msg->kill = true;
				break;
			}
			default: {
				break;
			}
		}
	}
}

void RenderEngine::updateBuffers(GLFWwindow* context, int width, int height) {
	this->context = context;
	this->width = width;
	this->height = height;

	glfwSetWindowSize(context,width,height);
	glViewport(0,0,width,height);
	dbg.setWindowSize(width,height);
	buffer.setBufferSizes(width,height);
	cam.updateDim(width,height);
	initTextures(width,height);
}

void RenderEngine::parseImportFile(const std::string& path) {
	Assimp::Importer imp;

	const aiScene* scene = imp.ReadFile( path,
											  aiProcess_Triangulate            |
											  aiProcess_JoinIdenticalVertices  |
											  aiProcess_GenNormals);


	if( !scene)
	{
		std::cout << imp.GetErrorString() << std::endl;
		return;
	}

	for(int i = 0; i < scene->mNumMeshes; i++) {
		meshes.push_back(MeshObject(scene->mMeshes[i],scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));
	}
}

int RenderEngine::getWidth() {
	return width;
}

int RenderEngine::getHeight() {
	return height;
}

void RenderEngine::setContext(GLFWwindow *window) {
	context = window;

	glViewport(0,0,width,height);
	glClearColor(0,0,0,1);
}

void RenderEngine::initTextures(int width, int height) {
	glDeleteTextures(1,&radTot);
	glDeleteTextures(1,&radPrev);
	glDeleteTextures(1,&alchAO);
	glDeleteTextures(1,&edges);
	glGenTextures(1, &radTot);

	glBindTexture(GL_TEXTURE_2D_ARRAY,radTot);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R11F_G11F_B10F,width,height,2,0,GL_RGB,GL_FLOAT,0);

	glGenTextures(1, &radPrev);
	glBindTexture(GL_TEXTURE_2D_ARRAY,radPrev);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R11F_G11F_B10F,width,height,2,0,GL_RGB,GL_FLOAT,0);

	glGenTextures(1, &alchAO);
	glBindTexture(GL_TEXTURE_2D,alchAO);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,width,height,0,GL_RED,GL_FLOAT,0);

	glGenTextures(1, &edges);
	glBindTexture(GL_TEXTURE_2D_ARRAY,edges);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RG8,width,height,2,0,GL_RG,GL_FLOAT,0);

	glBindTexture(GL_TEXTURE_2D,0);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
}


