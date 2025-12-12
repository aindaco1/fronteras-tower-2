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
    
    // Performance: disable vsync for smoother multi-window rendering
    // Only enable on window 0 to prevent screen tearing
    if (windowIndex == 0) {
        ofSetVerticalSync(true);
    } else {
        ofSetVerticalSync(false);
    }
    
    ofSetFrameRate(60);
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
            // Find which monitor this window is on
            int windowX, windowY;
            glfwGetWindowPos(glfwWindow, &windowX, &windowY);
            int centerX = windowX + 360;
            int centerY = windowY + 240;
            
            GLFWmonitor* targetMonitor = glfwGetPrimaryMonitor();
            int monitorCount;
            GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
            
            for (int i = 0; i < monitorCount; i++) {
                int monX, monY;
                glfwGetMonitorPos(monitors[i], &monX, &monY);
                const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
                
                if (centerX >= monX && centerX < monX + mode->width &&
                    centerY >= monY && centerY < monY + mode->height) {
                    targetMonitor = monitors[i];
                    break;
                }
            }
            
            // Get target monitor dimensions
            int monX, monY;
            glfwGetMonitorPos(targetMonitor, &monX, &monY);
            const GLFWvidmode* mode = glfwGetVideoMode(targetMonitor);
            
            // Remove window decoration and position to fill only this monitor
            glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
            glfwSetWindowPos(glfwWindow, monX, monY);
            glfwSetWindowSize(glfwWindow, mode->width, mode->height);
        } else {
            // Restore window decoration
            glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowPos(glfwWindow, 50, 50);
            glfwSetWindowSize(glfwWindow, 720, 480);
        }
        
        ofLogNotice() << "Window " << windowIndex << " borderless fullscreen: " << isFullscreen;
    }
}
