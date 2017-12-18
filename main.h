//
// Created by vbustamante on 04-Mar-17.
//

#ifndef SPACINVADERS_MAIN_H
#define SPACINVADERS_MAIN_H
#include <glm.hpp>

enum{
    shaderProgramMain,
    shaderProgramFont,
    shaderProgramVector,
    shaderProgramCount
};

void render_text(std::string text, GLfloat x, GLfloat y, GLfloat scale, ShaderProgram shader, glm::vec4 color);


const auto white = glm::vec4(1, 1, 1, 1);
const auto black = glm::vec4(0, 0, 0, 1);
const auto transparent_green = glm::vec4(0, 1, 0, 0.5);
const auto blue = glm::vec4(0, 0, 1, 1);
const auto red = glm::vec4(1, 0, 0, 1);

#endif //SPACINVADERS_MAIN_H
