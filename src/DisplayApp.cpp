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
            // Get the monitor the window is currently on
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            
            // Try to find the monitor containing the window center
            int windowX, windowY;
            glfwGetWindowPos(glfwWindow, &windowX, &windowY);
            int centerX = windowX + 360; // Assuming 720 width
            int centerY = windowY + 240; // Assuming 480 height
            
            // Find monitor containing this position
            int monitorCount;
            GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
            for (int i = 0; i < monitorCount; i++) {
                int monitorX, monitorY;
                glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);
                const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
                
                if (centerX >= monitorX && centerX < monitorX + mode->width &&
                    centerY >= monitorY && centerY < monitorY + mode->height) {
                    monitor = monitors[i];
                    break;
                }
            }
            
            // Set to fullscreen on detected monitor
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            // Exit fullscreen - return to windowed mode
            glfwSetWindowMonitor(glfwWindow, nullptr, 50, 50, 720, 480, GLFW_DONT_CARE);
        }
        
        ofLogNotice() << "Window " << windowIndex << " fullscreen: " << isFullscreen;
    }
}
