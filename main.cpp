#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <fstream>
#include <ctime>
#include <map>
#include <glm.hpp>
#include <ext.hpp>
#include <sstream>
#include <iomanip>

#include "shaderManager.h"
#include "State.h"
#include "main.h"

using namespace std;

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
State *activeState = nullptr;
GLFWwindow *window = nullptr;
ShaderProgram shaderSuite[shaderProgramCount];

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

template <class T> void setState(){
  if(activeState) delete activeState;
  activeState = new T(window, shaderSuite);
}

void render_text(std::string text, GLfloat x, GLfloat y, GLfloat scale, ShaderProgram shader, glm::vec4 color){

  static GLuint VBO = 0;
  // Lazy start VBO
  if(VBO == 0){
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glUseProgram(shader.id);

  glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
  GLint uniformProjection  = shaderGetUniform (shader.id, "projection");
  glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));


  GLint uniformFontColor= shaderGetUniform(shader.id, "color");
  glUniform4fv(uniformFontColor, 1, glm::value_ptr(color));
  glActiveTexture(GL_TEXTURE0);

  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++)
  {
    Character ch = Characters[*c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos,     ypos,       0.0, 1.0 },
        { xpos + w, ypos,       1.0, 1.0 },

        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos + w, ypos,       1.0, 1.0 },
        { xpos + w, ypos + h,   1.0, 0.0 }
    };
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  std::cout << "Changed Screen Size" << std::endl;
  glViewport(0, 0, width, height);
}


int main() {
  // INITIALIZE FOUNDATIONS
  if(!glfwInit()){
    cerr << "ERROR: Could not start glfw" << endl;
    return 1;
  }

  glfwWindowHint(GLFW_RESIZABLE, true);
  glfwWindowHint(GLFW_FOCUSED, false);
//  glfwWindowHint(GLFW_VISIBLE, false);
  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Spac Invaders", nullptr, nullptr);

  if (!window) {
    cerr << "Failed to open window with GLFW3" << endl;
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

  // Font Library Setup
  {
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

    FT_Set_Pixel_Sizes(fontFace, 0, 28);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(GLubyte c = 0; c<128; c++){
      if(FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
        cerr << "ERROR::FREETYTPE: Failed to load Glyph "<< (c+'a') << endl;
        continue;
      }

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          fontFace->glyph->bitmap.width,
          fontFace->glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          fontFace->glyph->bitmap.buffer
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Character character = {
          texture,
          glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
          glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
          fontFace->glyph->advance.x
      };
      Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(fontFace);
    FT_Done_FreeType(ft);

  }

  // SHADER SETUP
  setShaderProgramName(&shaderSuite[shaderProgramMain], "main");
  setShaderProgramName(&shaderSuite[shaderProgramFont], "font");
  setShaderProgramName(&shaderSuite[shaderProgramVector], "vector");
  shaderLoad(shaderSuite);

  // INFO DUMPS
  {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;
  }

  // APPLICATION VARIABLES
  double intervalTimer;
  time_t frameCounter = 0;
  const unsigned logInterval = 1000;
  bool changedScene= true;

  GLuint debugVao;
  glGenVertexArrays(1, &debugVao);
  glBindVertexArray(debugVao);


  setState<StateMenu>();

  // MAIN LOOP
  while(!glfwWindowShouldClose(window))
  {
    intervalTimer = glfwGetTime();
//    Process Input
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);


    glClearColor(0.2, 0.2, 0.2, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderHotLoad(shaderSuite);
//    changedScene = shaderHotLoad(shaderSuite);


    // Render GUI3
    activeState->render_ui();

    double frametime = glfwGetTime() - intervalTimer;
    auto sleepTime = static_cast<DWORD>((1/60.0f - frametime) * 1000);

    std::stringstream ss;
    ss << std::fixed;
    ss << std::setprecision(2);
    ss << 1/(frametime + (sleepTime/1000.0f)) << " FPS";
    render_text(ss.str(), 8.0f, 8.0f, .6f, shaderSuite[shaderProgramFont], black);

    //Call Callbacks and Refresh display
    glfwPollEvents();
    if(changedScene){
      changedScene = true;
      glfwSwapBuffers(window);
    }

    if(frametime < 1/60.0f && sleepTime < 100){
      Sleep(sleepTime);
    }
  }

  // CLEANUP
  glfwTerminate();

  return 0;
}