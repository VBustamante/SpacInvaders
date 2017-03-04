//
// Created by vbustamante on 03-Mar-17.
//

#ifndef SPACINVADERS_SHADERMANAGER_H
#define SPACINVADERS_SHADERMANAGER_H

struct Shader{
  char *text;
};

struct ShaderProgram{
    GLuint id;
    char *name;
};

void shaderLoad();
void shaderHotLoad(Shader[]);

#endif //SPACINVADERS_SHADERMANAGER_H
