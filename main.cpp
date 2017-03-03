#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

// Returns shaderId or 0 on error
GLuint loadShader(const char *shaderFilePath, GLuint shaderType){
  ifstream file;
  
  file.open(shaderFilePath, ifstream::in);

  if(!file.good()) return 0;

  // Calculate file size
  unsigned long fileSize;
  file.seekg(0, ios::end);
  fileSize = file.tellg();
  // Reset cursor
  file.seekg(ios::beg);

  GLchar *shaderText = new char[fileSize + 1];
  unsigned int fileCursor = 0;
  while(file.good()){
    shaderText[fileCursor] = (GLchar) file.get();
    if (!file.eof()) fileCursor++;
  }

  shaderText[fileCursor] = '\0';

  file.close();

  GLuint fragmentShaderId = glCreateShader(shaderType);
  glShaderSource(fragmentShaderId, 1, (const GLchar**)&shaderText, NULL);
  glCompileShader(fragmentShaderId);

  GLint shaderCompiled;
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderCompiled);

  if(shaderCompiled == GL_FALSE){
    GLint maxLength = 0;
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar *errorLog = new char[maxLength];
    glGetShaderInfoLog(fragmentShaderId, maxLength, &maxLength, errorLog);

    cerr << "Shader Error: " << endl << errorLog;

    glDeleteShader(fragmentShaderId);
    fragmentShaderId = 0;
  }
  return fragmentShaderId;
}

//get shader directory
const char *shaderRelativePath= "\\..\\shaders";
HANDLE reloadShadersAndStartWatcher(GLuint* shaderProgramId, HANDLE *shaderModificationHandler){

  unsigned shaderDirLength = GetCurrentDirectory(0, NULL);
  unsigned shaderRelativePathLength = strlen(shaderRelativePath);
  GLchar *shaderDir = new char[ shaderDirLength + shaderRelativePathLength ];
  GetCurrentDirectory(shaderDirLength, shaderDir);
  for(int i = 0; i< shaderRelativePathLength; i++){
    shaderDir[shaderDirLength + i - 1] = shaderRelativePath[i];
  }
  shaderDir[shaderDirLength + shaderRelativePathLength + 1] = '\0';

  if(*shaderModificationHandler){
    cout<<"old handle"<< endl;
    if(!FindNextChangeNotification(*shaderModificationHandler)){
      cout<<"old handle"<< endl;

    };
  }else{
    cout << "New handle" << endl;
    *shaderModificationHandler = FindFirstChangeNotification(
        shaderDir,
        FALSE, // Don't watch recursively
        FILE_NOTIFY_CHANGE_LAST_WRITE);

    if (*shaderModificationHandler == INVALID_HANDLE_VALUE)
    {
      cout <<  "ERROR: FindFirstChangeNotification function failed.\n" ;
      cout << GetLastError();
    }
  }

  // COMPILE SHADERS
  cout << "Compiling fragment shader" << endl;
  GLuint fragmentShaderId = loadShader("../shaders/mainF.glsl", GL_FRAGMENT_SHADER);
  cout << "Compiling vertex shader" << endl;
  GLuint vertexShaderId = loadShader("../shaders/mainV.glsl", GL_VERTEX_SHADER);

  if(!vertexShaderId || !fragmentShaderId){
    cerr << "Failed to compile a shader" << endl;
  }

  if(*shaderProgramId > 0) glDeleteProgram(*shaderProgramId);
  *shaderProgramId = glCreateProgram();
  glAttachShader(*shaderProgramId, vertexShaderId);
  glAttachShader(*shaderProgramId, fragmentShaderId);
  glLinkProgram(*shaderProgramId);

  return shaderModificationHandler;
}

int main() {
  // INITIALIZE FOUNDATIONS
  if(!glfwInit()){
    cerr << "ERROR: Could not start glfw" << endl;
    return 1;
  }

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

  // SHADER SETUP
  GLuint shaderProgram = 0;
  HANDLE handleShaderRefresh = NULL;
  reloadShadersAndStartWatcher(&shaderProgram, &handleShaderRefresh);
  glUseProgram(shaderProgram);

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
  const unsigned logInterval = 1000;

  // MAIN LOOP
  while(!glfwWindowShouldClose(window))
  {
    glClearColor(.5f, 0.0f, .0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //SHADER HOTLOADING
    DWORD waitStatus = WaitForSingleObject(handleShaderRefresh, 0);
    if(waitStatus == WAIT_OBJECT_0){
      cout << "changed shader" << endl;
      reloadShadersAndStartWatcher(&shaderProgram, &handleShaderRefresh);
      glUseProgram(shaderProgram);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);



    //Call Callbacks and Refresh display
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}