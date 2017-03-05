#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <fstream>
#include <ctime>

#include "shaderManager.h"
#include "main.h"

using namespace std;

int main() {
  // INITIALIZE FOUNDATIONS
  if(!glfwInit()){
    cerr << "ERROR: Could not start glfw" << endl;
    return 1;
  }

  glfwWindowHint(GLFW_RESIZABLE, false);
  glfwWindowHint(GLFW_FOCUSED, false);
//  glfwWindowHint(GLFW_VISIBLE, false);
  GLFWwindow *window = glfwCreateWindow(640, 420, "Spac Invaders", NULL, NULL);

  if (!window) {
    cerr << "Failed to open window with GLFW3" << endl;
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  if (gl3wInit()) {
    cerr << "Failed to initialize OpenGL" << endl;
    return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
    cerr << "OpenGL 3.2 not supported" << endl;
    return -1;
  }

  // INITIAL CONFIGURATIONS
  glfwSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // VERTICES SETUP
  float points[] = {
    0.f, 0.f, 0.f,
    1.f, 1.f, 0.f,
    1.f, 0.f, 0.f
  };

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 9* sizeof(float), points, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  // Font Library Setup
  FT_Library ft;
  if(FT_Init_FreeType(&ft)) {
    cout << "Failed to init freetype library" <<endl;
    return 1;
  }

  FT_Face fontFace;
  if(FT_New_Face(ft, "fonts/UbuntuMono-R.ttf", 0, &fontFace)){
    cout << "Failed to load font" <<endl;
    return 1;
  }

  // SHADER SETUP
  ShaderProgram shaderSuite[shaderProgramCount];

  setShaderProgramName(&shaderSuite[shaderProgramMain], "main");
  setShaderProgramName(&shaderSuite[shaderProgramFont], "font");
  shaderLoad(shaderSuite);

  // INFO DUMPS
  {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;
  }

  // APPLICATION VARIABLES
  time_t startup = time(NULL);
  time_t intervalTimer;
  time_t fileChanged = startup;
  time_t frameCounter = 0;
  const unsigned logInterval = 1000;

  // MAIN LOOP
  while(!glfwWindowShouldClose(window))
  {
    //glClearColor(.5f, 0.0f, .0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //SHADER HOTLOADING
    shaderHotLoad(shaderSuite);
    glUseProgram(shaderSuite[shaderProgramFont].id);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Call Callbacks and Refresh display
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // CLEANUP
  glfwTerminate();

  return 0;
}