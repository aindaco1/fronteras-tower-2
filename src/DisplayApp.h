#pragma once

#include "ofMain.h"
#include "DisplayManager.h"

class DisplayApp : public ofBaseApp {
public:
    void setup(DisplayManager* mgr, int windowIdx);
    void update();
    void draw();
    
private:
    DisplayManager* manager;
    int windowIndex;
};
