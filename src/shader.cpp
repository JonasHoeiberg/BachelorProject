#include "shader.h"

#include <fstream>
#include <iostream>

using namespace Shader;

std::string ShaderProgram::getUniformString(U_Ref uniform){
	switch(uniform) {
		case U_M_MODEL:
			return "model";
		case U_M_VIEW:
			return "view";
		case U_M_PROJECTION:
			return "projection";
		case U_M_PVM:
			return "pvm";
		case U_M_VM:
			return "vm";
		case U_M_VUNPROJECT:
			return "viewUnproject";
		case U_M_UNVIEW:
			return "invView";
		case U_F_FARPLANE:
			return "far_plane";
		case U_F_NEARPLANE:
			return "near_plane";
		case U_I_TEXTURE:
			return "texture_uniform";
		case U_I_DIFF_TEX:
			return "diff_texture";
		case U_I_NORM_TEX:
			return "norm_texture";
		case U_I_SPEC_TEX:
			return "spec_texture";
		case U_I_DEPTH_TEX:
			return "depth_texture";
		case U_I_NOISE_TEX:
			return "noise_tex";
		case U_I_PREVRAD:
			return "prev_bounce";
		case U_I_EDGE_TEX:
			return "edge_texture";
		case U_I_LAYER:
			return "layer";
		case U_I_SHADOWMAP:
			return "shadowMap";
		case U_I_AO:
			return "ao_tex";
		case U_I_RAD:
			return "rad_tex";
		case U_V4_DIFF_COL:
			return "diff_color";
		case U_V4_SPEC_COL:
			return "spec_color";
		case U_I_TEX_X_OFFSET:
			return "x_offset";
		case U_I_GLYPH_WIDTH:
			return "glyph_width";
		case U_I_GLYPH_HEIGHT:
			return "glyph_height";
		case U_V3_COLOR:
			return "color";
		case U_V3_EYEPOS:
			return "eyePos";
		case U_V3_LIGHTPOS:
			return "light_pos";
		case U_V3_LIGHT_INTENSITY:
			return "light_intensity";
		case U_V2_PLANE_DIM:
			return "plane_dim";
		case U_V2ARRAY_SAMPLES:
			return "samplePoints";
		default:
			printf("You're not supposed to be seeing this!");
			return "";
	}
}


ShaderProgram::ShaderProgram(const std::string& path_to_vert,
							 const std::string& path_to_frag,
							 const std::string& path_to_geom) {

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, path_to_vert);
	GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, path_to_frag);


	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	if(!path_to_geom.empty()) {
		GLuint geomShader = compileShader(GL_GEOMETRY_SHADER, path_to_geom);
		glAttachShader(program, geomShader);
	}
	glLinkProgram(program);

	GLint result = GL_FALSE;
	int logLength = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		std::cout << "A linking issue occured with " << path_to_vert << " and " << path_to_frag;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> programError((logLength > 1) ? logLength : 1);
		glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
		std::cout << &programError[0] << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	glUseProgram(program);

	for(int i = 0; i != U_LAST; i++) {
		GLint loc = glGetUniformLocation(program,getUniformString((U_Ref) i).c_str());

		if(loc != -1) {
			uniforms.insert(std::pair<U_Ref, GLuint>((U_Ref) i, loc));
		}

	}

}

std::string ShaderProgram::readFile(const std::string& filePath) {
	std::string output;
	std::ifstream fileStream;
	fileStream.open(filePath, std::ios::in);

	if(!fileStream.is_open())
		std::cout << "Error reading shader file at " << filePath << std::endl;

	std::string line;
	while(!fileStream.eof()) {
		std::getline(fileStream,line);
		output.append(line + "\n");
	}

	fileStream.close();

	return output;
}

void ShaderProgram::checkCompileError(GLuint shader, const std::string& filename) {
	GLint result = GL_FALSE;
	int logLength = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		std::cout << "A shader " << filename << " did not compile" << std::endl;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
		std::cout << &vertShaderError[0] << std::endl;
	}
}

GLuint ShaderProgram::compileShader(GLuint shaderType,const std::string& path) {
	std::string shaderString = readFile(path);

	const char* shaderSource = shaderString.c_str();

	GLuint shader;

	shader = glCreateShader(shaderType);
	glShaderSource(shader,1,&shaderSource,NULL);
	glCompileShader(shader);

	checkCompileError(shader, path);

	return shader;
}

void ShaderProgram::use() {
	glUseProgram(program);
}

GLuint ShaderProgram::get_U_Location(U_Ref reference) {
	std::map<U_Ref,GLuint>::iterator it = uniforms.find(reference);
	if(it != uniforms.end())
		return it->second;
	else {
		std::cout << "Uniform " << reference << " was requested but not found" << std::endl;
		return 0;
	}
}

ShaderProgram::ShaderProgram() {

}
