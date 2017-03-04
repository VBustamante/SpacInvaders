//
// Created by vbustamante on 03-Mar-17.
//
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "shaderManager.h"

using namespace std;


void shaderLoad();

void shaderHotLoad(Shader shaderSuite[]){
  static HANDLE shaderModificationHandler = NULL;
  static bool shaderChangeNotified = false;
  static unsigned frameCounter = 0;

  if(!shaderChangeNotified){
    {
      if(!shaderModificationHandler){
        const char *shaderRelativePath= "\\..\\shaders";
        unsigned shaderDirLength = GetCurrentDirectory(0, NULL);
        unsigned shaderRelativePathLength = strlen(shaderRelativePath);
        GLchar *shaderDir = new char[ shaderDirLength + shaderRelativePathLength ];
        GetCurrentDirectory(shaderDirLength, shaderDir);
        for(int i = 0; i< shaderRelativePathLength; i++){
          shaderDir[shaderDirLength + i - 1] = shaderRelativePath[i];
        }
        shaderDir[shaderDirLength + shaderRelativePathLength - 1] = '\0';

        shaderModificationHandler = FindFirstChangeNotification(
            shaderDir, //TODO Receive from build system
            FALSE, // Don't watch recursively
            FILE_NOTIFY_CHANGE_SIZE);

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
//        fileChanged = time(NULL);
        frameCounter = 0;
      }
    }
  }
  // We have to wait till the files have changed after
  // the Handle notifies us that the files have changed.
  // Yup.
  if(shaderChangeNotified){
    // TODO shader comparison and reloading
    if(true){// Check real change
      if (!FindNextChangeNotification(shaderModificationHandler)) {
        cout << "ERROR: Error setting handler" << endl;
        cout << GetLastError() << endl;
      };

      shaderChangeNotified = false;
    }
    frameCounter++;
  }
}

void shaderManager(){
}