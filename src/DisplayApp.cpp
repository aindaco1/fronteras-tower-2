#include "DisplayApp.h"

void DisplayApp::init(DisplayManager* mgr, int windowIdx) {
    manager = mgr;
    windowIndex = windowIdx;
}

void DisplayApp::setup() {
    // GL context not ready yet, defer to update
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
        isFullscreen = !isFullscreen;
        ofSetFullscreen(isFullscreen);
        ofLogNotice() << "Window " << windowIndex << " fullscreen: " << isFullscreen;
    }
}
