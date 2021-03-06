//
// Created by vbustamante on 03-Mar-17.
//
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <fstream>

#include "shaderManager.h"
#include "main.h"

using namespace std;
const char *shaderRelativePath= "..\\shaders";

void setShaderProgramName(ShaderProgram *sp, const char *name){
  sp->name = new char [strlen(name) + 1];
  strcpy(sp->name, name);
}

void shaderLoad(ShaderProgram shaderSuite[]){
  ShaderProgram *shaderProgram;
  for(int i =0; i < shaderProgramCount; i++){
    shaderProgram = &shaderSuite[i];
    cout << "loading " << shaderProgram->name << " shader program" << endl;

    GLchar *vertexShader = stringifyShader(shaderSuite[i].name, "V");
    GLuint vertexShaderId = 0;
    if(vertexShader != nullptr){
      vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertexShaderId, 1, (const GLchar**) &vertexShader, nullptr);
      glCompileShader(vertexShaderId);

      GLint shaderCompilerStatus;
      glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &shaderCompilerStatus);

      if(shaderCompilerStatus == GL_FALSE){
        GLint maxLength = 0;
        glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &maxLength);

        GLchar *errorLog = new char[maxLength];
        glGetShaderInfoLog(vertexShaderId, maxLength, &maxLength, errorLog);

        cerr << shaderSuite[i].name<<"'s vertex shader error:"<< endl << errorLog;
        delete []errorLog;
        glDeleteShader(vertexShaderId);
        vertexShaderId = 0;
      }
    }
    shaderProgram->vertexShader.id = vertexShaderId;
    shaderProgram->vertexShader.text = vertexShader;

    GLchar *fragmentShader = stringifyShader(shaderSuite[i].name, "F");
    GLuint fragmentShaderId = 0;
    if(fragmentShader != nullptr){
      fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragmentShaderId, 1, (const GLchar**) &fragmentShader, nullptr);
      glCompileShader(fragmentShaderId);

      GLint shaderCompilerStatus;
      glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderCompilerStatus);

      if(shaderCompilerStatus == GL_FALSE){
        GLint maxLength = 0;
        glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &maxLength);

        GLchar *errorLog = new char[maxLength];
        glGetShaderInfoLog(fragmentShaderId, maxLength, &maxLength, errorLog);

        cerr << shaderSuite[i].name<<"'s fragment shader error:"<< endl << errorLog;
        delete []errorLog;
        glDeleteShader(fragmentShaderId);
        fragmentShaderId = 0;
      }
    }
    shaderProgram->fragmentShader.id = fragmentShaderId;
    shaderProgram->fragmentShader.text = fragmentShader;


    if(shaderProgram->id != 0) glDeleteProgram(shaderProgram->id);
    shaderProgram->id = glCreateProgram();
    if(shaderProgram->vertexShader.id) glAttachShader(shaderProgram->id, shaderProgram->vertexShader.id);
    if(shaderProgram->fragmentShader.id) glAttachShader(shaderProgram->id, shaderProgram->fragmentShader.id);
    glLinkProgram(shaderProgram->id);
  }
}

bool shaderHotLoad(ShaderProgram shaderSuite[]){
  static HANDLE shaderModificationHandler = nullptr;
  static bool shaderChangeNotified = false;
  static unsigned frameCounter = 0;
  bool shaderChanged = shaderModificationHandler == nullptr;

  if(!shaderChangeNotified){
    {
      if(!shaderModificationHandler){
        unsigned shaderDirLength = GetCurrentDirectory(0, nullptr);
        unsigned shaderRelativePathLength = strlen(shaderRelativePath);
        auto *shaderDir = new char[ shaderDirLength + shaderRelativePathLength + 1];
        GetCurrentDirectory(shaderDirLength, shaderDir);
        shaderDir[shaderDirLength - 1] = '\\';

        for(int i = 0; i< shaderRelativePathLength; i++){
          shaderDir[shaderDirLength + i ] = shaderRelativePath[i];
        }

        shaderDir[shaderDirLength + shaderRelativePathLength ] = '\0';

        shaderModificationHandler = FindFirstChangeNotification(
            shaderDir, //TODO Receive from build system
            FALSE, // Don't watch recursively
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_FILE_NAME);

        if (shaderModificationHandler == INVALID_HANDLE_VALUE)
        {
          cout <<  "ERROR: Error setting handler" << endl;
          cout << GetLastError() << endl;
        }
      }

      DWORD waitStatus = WaitForSingleObject(shaderModificationHandler, 0);
      if(waitStatus == WAIT_OBJECT_0){
        cout << "changed shader" << endl;
        shaderChangeNotified = true;
        frameCounter = 0;
      }
    }
  }
  // We have to wait till the files have changed after
  // the Handle notifies us that the files have changed.
  // Yup.
  if(shaderChangeNotified){
    bool anyFileChanged = false;

    for(int i =0; i < shaderProgramCount; i++){
      ShaderProgram *shaderProgram = &shaderSuite[i];
      if(shaderProgram->vertexShader.id != 0){
        GLchar *newShader = stringifyShader(shaderSuite[i].name, "V");
        if(newShader == nullptr) cout << "deleted "<< shaderProgram->name << "'s vertex shader" << endl;
        if(newShader == nullptr || strcmp(shaderSuite[i].vertexShader.text, newShader) != 0) anyFileChanged = true;
      }
      if(shaderProgram->fragmentShader.id != 0){
        GLchar *newShader = stringifyShader(shaderProgram->name, "F");
        if(newShader == nullptr) cout << "deleted "<< shaderProgram->name << "'s fragment shader" << endl;
        if(newShader == nullptr || strcmp(shaderProgram->fragmentShader.text, newShader) != 0) anyFileChanged = true;
      }
    }

    if(anyFileChanged || frameCounter >= 10){// Check real change

      shaderLoad(shaderSuite);

      cout << "took "<<frameCounter<<" frames"<<endl;

      if (!FindNextChangeNotification(shaderModificationHandler)) {
        cout << "ERROR: Error setting handler" << endl;
        cout << GetLastError() << endl;
      };
      shaderChangeNotified = false;
      shaderChanged = true;
    }
    frameCounter++;
  }
  return shaderChanged;
}

GLint shaderGetAttrib(GLuint program, const char *name) {
  GLint attribute = glGetAttribLocation(program, name);
  if(attribute == -1)
    fprintf(stderr, "Could not bind attribute %s\n", name);
  return attribute;
}

GLint shaderGetUniform(GLuint program, const char *name) {
  GLint uniform = glGetUniformLocation(program, name);
  if(uniform == -1)
    fprintf(stderr, "Could not bind uniform %s\n", name);
  return uniform;
}

GLchar *stringifyShader(const char *shaderName, const char *shaderType){

  auto *filePath = new char[strlen(shaderRelativePath) + 1 + strlen(shaderName) + strlen(shaderType) + 6];
  strcpy(filePath, shaderRelativePath);
  strcpy(&filePath[strlen(shaderRelativePath)], "\\");
  strcpy(&filePath[strlen(shaderRelativePath) + 1], shaderName);
  strcpy(&filePath[strlen(shaderRelativePath) + 1 + strlen(shaderName)], shaderType);
  strcpy(&filePath[strlen(shaderRelativePath) + 1 + strlen(shaderName) + strlen(shaderType)], ".glsl");


  ifstream file;
  file.open(filePath, ifstream::in);
  if(!file.good()){
    file.close();
    return nullptr;
  }
  // Calculate file size
  unsigned long fileSize;
  file.seekg(0, ios::end);
  fileSize = file.tellg();
  // Reset cursor
  file.seekg(ios::beg);

  GLchar *ShaderText = new char[fileSize + 1];
  unsigned int fileCursor = 0;
  while(file.good()){
    ShaderText[fileCursor] = (GLchar) file.get();
    if (!file.eof()) fileCursor++;
  }
  ShaderText[fileCursor] = '\0';
  file.close();
  delete[] filePath;

  return ShaderText;
}