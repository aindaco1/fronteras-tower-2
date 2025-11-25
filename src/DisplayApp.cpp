#include "DisplayApp.h"

void DisplayApp::init(DisplayManager* mgr, int windowIdx) {
    manager = mgr;
    windowIndex = windowIdx;
}

void DisplayApp::setGlfwWindow(GLFWwindow* win) {
    glfwWindow = win;
}

void DisplayApp::setup() {
    // Set GLFW window pointer when GL context is ready
    if (!glfwWindow) {
        glfwWindow = glfwGetCurrentContext();
        ofLogNotice() << "Window " << windowIndex << " set GLFW context";
    }
}

void DisplayApp::update() {
    // Only window 0 initializes and updates the manager
    if(!initialized && windowIndex == 0 && manager) {
        manager->setup();
        initialized = true;
    }
    if(windowIndex == 0 && manager) {
        manager->update();
    }
}

void DisplayApp::draw() {
    ofBackground(0);
    // All windows share the same manager and draw
    if(manager && manager->isSetup()) {
        manager->draw(windowIndex);
    } else {
        // Debug: show which window this is if manager not ready
        ofSetColor(255);
        ofDrawBitmapString("Window " + ofToString(windowIndex) + " waiting for setup...", 20, 20);
    }
}

void DisplayApp::keyPressed(int key) {
    // 'f' key toggles fullscreen/borderless on this window
    if (key == 'f' || key == 'F') {
        if (!glfwWindow) return;
        
        isFullscreen = !isFullscreen;
        
        if (isFullscreen) {
            // Remove window decoration (title bar, borders)
            glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
            // Maximize the window to fill the monitor
            glfwMaximizeWindow(glfwWindow);
        } else {
            // Restore window decoration
            glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_TRUE);
            // Restore to smaller window size
            glfwRestoreWindow(glfwWindow);
            glfwSetWindowPos(glfwWindow, 50, 50);
            glfwSetWindowSize(glfwWindow, 720, 480);
        }
        
        ofLogNotice() << "Window " << windowIndex << " borderless fullscreen: " << isFullscreen;
    }
}
