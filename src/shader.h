#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include "mat4x4.hpp"
#include "vec3.hpp"
#include <string>
#include <map>
#include <vector>
namespace Shader {

enum U_Ref {
    U_M_MODEL,
    U_M_VIEW,
    U_M_PROJECTION,
    U_M_PVM,
    U_M_VM,
    U_M_VUNPROJECT,
    U_M_UNVIEW,
    U_F_NEARPLANE,
    U_F_FARPLANE,
    U_I_TEXTURE,
    U_I_DIFF_TEX,
    U_I_SPEC_TEX,
    U_I_DEPTH_TEX,
    U_I_NORM_TEX,
    U_I_NOISE_TEX,
    U_I_LAYER,
    U_I_PREVRAD,
    U_I_SHADOWMAP,
	U_I_AO,
	U_I_RAD,
	U_I_EDGE_TEX,
    U_V4_DIFF_COL,
    U_V4_SPEC_COL,
    U_I_TEX_X_OFFSET,
    U_I_GLYPH_HEIGHT,
    U_I_GLYPH_WIDTH,
    U_V3_COLOR,
    U_V3_EYEPOS,
    U_V3_LIGHTPOS,
    U_V3_LIGHT_INTENSITY,
    U_V2_PLANE_DIM,
    U_V2ARRAY_SAMPLES,
    U_LAST
};  //Remember to add uniforms to the string list in getUniformString()!!

class ShaderProgram {
public:
    ShaderProgram(const std::string& path_to_vert,
                  const std::string& path_to_frag,
                  const std::string& path_to_geom = std::string());
    ShaderProgram();

    void use();
    GLuint get_U_Location(U_Ref reference);
private:
    GLuint program;
    std::map<U_Ref,GLuint> uniforms;

    std::string readFile(const std::string& filePath);
    GLuint compileShader(GLuint shaderType,const std::string& path);
    void checkCompileError(GLuint shader, const std::string& filename);
    std::string getUniformString(U_Ref uniform);
};

}

#endif //SHADER_H