#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

const char* vertex_shader =
  "#version, 120\n"
  "in vec3 vp;"
  "void main(){"
  "   gl_Position = vec4(vp, 1.0)"
  "}";

const char* fragment_shader =
    "#version, 120\n"
        "out vec4 frag_color;"
        "void main(){"
        "   frag_color = vec4(.5, 0, .5 1.0)"
        "}";


int main() {
  if(!glfwInit()){
    fprintf(stderr, "ERROR: Could not start glfw\n");
    return 1;
  }

  GLFWwindow *window = glfwCreateWindow(640, 420, "Spac Invaders", NULL, NULL);

  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);


  if (gl3wInit()) {
    fprintf(stderr, "failed to initialize OpenGL\n");
    return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
    fprintf(stderr, "OpenGL 3.2 not supported\n");
    return -1;
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("GL Version: %s\n", version);

  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);

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

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &vertex_shader, NULL);
  glCompileShader(fs);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, fs);
  glAttachShader(shaderProgram, vs);
  glLinkProgram(shaderProgram);

  glClearColor(.5f, 0.0f, .0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);


  //glfwSwapInterval(1);
  while(!glfwWindowShouldClose(window))
  {

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}