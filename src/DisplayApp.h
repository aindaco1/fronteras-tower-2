#pragma once

#include "ofMain.h"
#include "DisplayManager.h"
#include <GLFW/glfw3.h>

class DisplayApp : public ofBaseApp {
public:
    void init(DisplayManager* mgr, int windowIdx);
    void setGlfwWindow(GLFWwindow* win);
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    
private:
    DisplayManager* manager = nullptr;
    int windowIndex = -1;
    bool initialized = false;
    bool isFullscreen = false;
    GLFWwindow* glfwWindow = nullptr;
};
