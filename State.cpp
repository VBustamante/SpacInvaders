//
// Created by vbustamante on 17-Dec-17.
//

#include <GL/gl3w.h>
#include <functional>
#include <sstream>
#include "State.h"
#include "main.h"

State::State(GLFWwindow *window, ShaderProgram *shaders): window(window), shaders(shaders){ glGenVertexArrays(1, &VAO); }

StateMenu::StateMenu(GLFWwindow *window, ShaderProgram *shaders): State(window, shaders){
  glfwSetKeyCallback(window, get_key_callback());
  glfwSetCharCallback(window, get_character_callback());
}

GLFWkeyfun StateMenu::get_key_callback() {

  glfwSetWindowUserPointer(window, this);

  return [](GLFWwindow* window, int key, int scancode, int action, int mods){
    auto& self = *static_cast<StateMenu*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT) && self.logString.length()){
      if(mods == GLFW_MOD_CONTROL) self.logString.clear();
      else self.logString.pop_back();
    }
  };

}

GLFWcharfun StateMenu::get_character_callback() {
  glfwSetWindowUserPointer(window, this);

  return [](GLFWwindow* window, unsigned int key){
    auto& self = *static_cast<StateMenu*>(glfwGetWindowUserPointer(window));

    self.logString.push_back((char) key);
  };

}

void StateMenu::render_ui() {
  render_text("8 Points", 25.0f, 560.0f, 1.0f, shaders[shaderProgramFont], black);
  render_text(logString, 25.0f, 25.0f, 1.0f, shaders[shaderProgramFont], white);
}
