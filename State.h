//
// Created by vbustamante on 17-Dec-17.
//

#ifndef SPACINVADERS_STATE_H
#define SPACINVADERS_STATE_H

#include <iostream>
#include <GLFW/glfw3.h>
#include "shaderManager.h"

class State {
public:
  explicit State(GLFWwindow *, ShaderProgram *);

  virtual void render_ui()= 0;
protected:
  GLFWwindow *window;
  ShaderProgram *shaders;
  GLuint VAO, VBO;
};

class StateMenu: public State{
public:

  StateMenu(GLFWwindow *window, ShaderProgram *shaders);

  GLFWkeyfun get_key_callback();
  GLFWcharfun get_character_callback();
  void render_ui() override;

private:
  std::string logString;
};


#endif //SPACINVADERS_STATE_H
