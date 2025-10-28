#include "DisplayApp.h"

void DisplayApp::init(DisplayManager* mgr, int windowIdx) {
    manager = mgr;
    windowIndex = windowIdx;
}

void DisplayApp::setup() {
    // GL context not ready yet, defer to update
}

void DisplayApp::update() {
    if(!initialized && windowIndex == 0 && manager) {
        manager->setup();
        initialized = true;
    }
    if(windowIndex == 0 && manager && initialized) {
        manager->update();
    }
}

void DisplayApp::draw() {
    ofBackground(0);
    if(manager) {
        manager->draw(windowIndex);
    }
}
