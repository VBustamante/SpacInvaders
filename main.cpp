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
//  glEnable(GL_DEPTH_TEST);
//  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // VERTICES SETUP
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  /*
  float points[] = {
      0.f, 0.f,
      0.f, 1.f,
      .5f, 1.f
  };
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 12* sizeof(float), points, GL_STATIC_DRAW);
  //glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  */

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

  FT_Set_Pixel_Sizes(fontFace, 0, 48);

  // SHADER SETUP
  ShaderProgram shaderSuite[shaderProgramCount];

  setShaderProgramName(&shaderSuite[shaderProgramMain], "main");
  setShaderProgramName(&shaderSuite[shaderProgramFont], "font");
  setShaderProgramName(&shaderSuite[shaderProgramVector], "vector");
  shaderLoad(shaderSuite);

  GLint uniformFontColor= shaderGetUniform(shaderSuite[shaderProgramFont].id, "color");
  GLint uniformTex      = shaderGetUniform(shaderSuite[shaderProgramFont].id, "tex");
  GLint attributeCoord  = shaderGetAttrib (shaderSuite[shaderProgramFont].id, "coord");


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

  GLfloat black[4] = { 0, 0, 0, 1 };
  GLfloat red[4] = { 1, 0, 0, 1 };
  GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  // MAIN LOOP
  while(!glfwWindowShouldClose(window))
  {
    glClearColor(1, 0, 1, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderHotLoad(shaderSuite);

    // Render GUI


    GLfloat blue[4] = {0, 0, 1, 1};
    glUniform4fv(uniformFontColor, 1, blue);

    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniformTex, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnableVertexAttribArray(attributeCoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attributeCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    FT_GlyphSlot g = fontFace->glyph;

    if(FT_Load_Char(fontFace, 'X', FT_LOAD_RENDER)) {
      cerr << "Could not load character 'X'" << endl;
      return 1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);



    GLfloat box[] = {
        0.f, 0.f, 0.f, 0.f,
        .5f, 0.f, 1.f, 0.f,
        0.f, .5f, 0.f, 1.f,
        .5f, .5f, 1.f, 1.f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);

    glUseProgram(shaderSuite[shaderProgramMain].id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


    glUseProgram(shaderSuite[shaderProgramFont].id);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



    glDisableVertexAttribArray(attributeCoord);
    glDeleteTextures(1, &tex);


    //Call Callbacks and Refresh display
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // CLEANUP
  glfwTerminate();

  return 0;
}