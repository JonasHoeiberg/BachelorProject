//
// Created by jonas on 3/23/15.
//

#include "TextRender.h"
#include "shader.h"

#include <vector>
#include <iostream>
#include <ft2build.h>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include FT_FREETYPE_H

TextRender::TextRender(std::string font, int fontSize, unsigned int screen_width, unsigned int screen_height) :
mesh(),
shader("../shaders/text.vert","../shaders/text.frag")
{
    sWidth = screen_width;
    sHeight = screen_height;

    fontTexture = setupText(font,fontSize);

    this->fontSize = fontSize;

    projection = glm::ortho(0.0f,(float) sWidth,0.0f,(float) sHeight,0.1f,3.0f);

    std::vector<Vertex> vertices =
            {
                    Vertex(glm::vec3(0.0f,0.0f,0.0f),glm::normalize(glm::vec3(0,0,1)),glm::vec2(0.0f,0.0f)),
                    Vertex(glm::vec3(0.0f,1.0f,0.0f),glm::normalize(glm::vec3(0,0,1)),glm::vec2(0.0f,1.0f)),
                    Vertex(glm::vec3(1.0f,0.0f,0.0f),glm::normalize(glm::vec3(0,0,1)),glm::vec2(1.0f,0.0f)),
                    Vertex(glm::vec3(1.0f,1.0f,0.0f),glm::normalize(glm::vec3(0,0,1)),glm::vec2(1.0f,1.0f))
            };

    std::vector<GLuint> indices = {0,1,2,3};

    mesh = MeshObject(vertices,indices);
}

GLuint TextRender::setupText(std::string font, int fontSize) {
    FT_Library ft;

    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
    }

    FT_Face face;

    if(FT_New_Face(ft, font.c_str(), 0, &face)) {
        fprintf(stderr, "Could not open font\n");
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    FT_GlyphSlot g = face->glyph;
    unsigned int w = 0;
    unsigned int h = 0;

    for(int i = 32; i < NUM_CHARS; i++) {
        if(FT_Load_Char(face, i,FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }

        w += g->bitmap.width;
        h = std::max(h, g->bitmap.rows);
    }

    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    int x = 0;

    for(int i = 32; i < NUM_CHARS; i++) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        glyphInfo[i].ax = g->advance.x >> 6;
        glyphInfo[i].ay = g->advance.y >> 6;

        glyphInfo[i].bw = g->bitmap.width;
        glyphInfo[i].bh = g->bitmap.rows;

        glyphInfo[i].bl = g->bitmap_left;
        glyphInfo[i].bt = g->bitmap_top;

        glyphInfo[i].tx = x;

        x += g->bitmap.width;
    }

    this->atlas_width = w;
    this->atlas_height = h;

    return tex;
}

void TextRender::renderChar(const char &c, int x, int y) {
    glDisable(GL_DEPTH_TEST);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    shader.use();

    glBindVertexArray(mesh.getVAO());

    glm::mat4 model = glm::translate(glm::vec3(x, y - (glyphInfo[c].bh - glyphInfo[c].bt),0.0f)) * glm::scale(glm::vec3(glyphInfo[c].bw,glyphInfo[c].bh,1.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0,0,1.0f),glm::vec3(0,0,-1.0f),glm::vec3(0,1,0.0f));

    glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_MODEL),1,GL_FALSE,glm::value_ptr(model));

    glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_VIEW),1,GL_FALSE,glm::value_ptr(view));

    glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_PROJECTION),1,GL_FALSE,glm::value_ptr(projection));

    glUniform1f(shader.get_U_Location(Shader::U_I_TEX_X_OFFSET),glyphInfo[c].tx / atlas_width);
    glUniform1f(shader.get_U_Location(Shader::U_I_GLYPH_WIDTH), glyphInfo[c].bw / atlas_width);
    glUniform1f(shader.get_U_Location(Shader::U_I_GLYPH_HEIGHT), glyphInfo[c].bh / atlas_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glUniform1i(shader.get_U_Location(Shader::U_I_TEXTURE), 0);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);

    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
}

void TextRender::renderString(const std::string& string, int x, int y, const glm::vec3& color) {
    glDisable(GL_DEPTH_TEST);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    shader.use();

    glBindVertexArray(mesh.getVAO());

    float xPos = x;
    float yPos = y;

    glm::mat4 model;
    glm::mat4 view = glm::lookAt(glm::vec3(0,0,1.0f),glm::vec3(0,0,-1.0f),glm::vec3(0,1,0.0f));

    glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_VIEW),1,GL_FALSE,glm::value_ptr(view));

    glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_PROJECTION),1,GL_FALSE,glm::value_ptr(projection));

    glUniform3fv(shader.get_U_Location(Shader::U_V3_COLOR),1,&color[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glUniform1i(shader.get_U_Location(Shader::U_I_TEXTURE), 0);

    for(int i = 0; i < string.length(); i++) {
        char c = string[i];

        if(c == '\n') {
            xPos = x;
            yPos -= fontSize;
            continue;
        }

        model = glm::translate(glm::vec3(xPos, yPos - (glyphInfo[c].bh - glyphInfo[c].bt),0.0f)) *
                glm::scale(glm::vec3(glyphInfo[c].bw,glyphInfo[c].bh,1.0f));

        glUniformMatrix4fv(shader.get_U_Location(Shader::U_M_MODEL),1,GL_FALSE,glm::value_ptr(model));

        glUniform1f(shader.get_U_Location(Shader::U_I_TEX_X_OFFSET),glyphInfo[c].tx / atlas_width);
        glUniform1f(shader.get_U_Location(Shader::U_I_GLYPH_WIDTH), glyphInfo[c].bw / atlas_width);
        glUniform1f(shader.get_U_Location(Shader::U_I_GLYPH_HEIGHT), glyphInfo[c].bh / atlas_height);

        xPos += glyphInfo[c].ax;
        yPos += glyphInfo[c].ay;

        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    }

    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
}

TextRender::TextRender() :
shader(){

}

void TextRender::setBufferSize(unsigned int width, unsigned int height) {
    sWidth = width;
    sHeight = height;

    projection = glm::ortho(0.0f,(float) sWidth,0.0f,(float) sHeight,0.1f,3.0f);
}
