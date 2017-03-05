//
// Created by vbustamante on 03-Mar-17.
//

#ifndef SPACINVADERS_SHADERMANAGER_H
#define SPACINVADERS_SHADERMANAGER_H

struct Shader{
  GLuint id = 0;
  char *text = NULL;
};

struct ShaderProgram{
    GLuint id = 0;
    char *name = NULL;
    Shader vertexShader;
    Shader fragmentShader;
};


void setShaderProgramName(ShaderProgram *sp, const char *name);
GLchar *stringifyShader(const char *shaderName, const char *shaderType);
void shaderLoad(ShaderProgram[]);
void shaderHotLoad(ShaderProgram[]);

#endif //SPACINVADERS_SHADERMANAGER_H
