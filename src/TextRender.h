//
// Created by jonas on 3/23/15.
//

#ifndef _GLFWTEST_TEXTRENDER_H_
#define _GLFWTEST_TEXTRENDER_H_

#include "shader.h"
#include "MeshObject.h"

#include <GL/glew.h>
#include <string>

class TextRender {
public:
    TextRender();
    TextRender(std::string font, int fontSize, unsigned int screen_width, unsigned int screen_height);

    void renderChar(const char& c, int x, int y);
    void renderString(const std::string& string, int x, int y, const glm::vec3& color);
    void setBufferSize(unsigned int width, unsigned int height);
private:
    GLuint fontTexture;
    MeshObject mesh;
    Shader::ShaderProgram shader;

    int atlas_width;
    int atlas_height;

    unsigned int sWidth;
    unsigned int sHeight;

    int fontSize;

    static const int NUM_CHARS = 256;

    glm::mat4 projection;

    GLuint setupText(std::string font, int fontSize);

    struct character_info {
        float ax; // advance.x
        float ay; // advance.y

        float bw; // bitmap.width;
        float bh; // bitmap.rows;

        float bl; // bitmap_left;
        float bt; // bitmap_top;

        float tx; // x offset of glyph in texture coordinates
    } glyphInfo[NUM_CHARS];
};


#endif //_GLFWTEST_TEXTRENDER_H_
