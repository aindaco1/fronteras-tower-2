#include "DisplayApp.h"

void DisplayApp::setup(DisplayManager* mgr, int windowIdx) {
    manager = mgr;
    windowIndex = windowIdx;
}

void DisplayApp::update() {
    if(windowIndex == 0) {
        manager->update();
    }
}

void DisplayApp::draw() {
    ofBackground(0);
    manager->draw(windowIndex);
}
