#pragma once

#include "ofMain.h"
#include "DisplayManager.h"

class DisplayApp : public ofBaseApp {
public:
    void init(DisplayManager* mgr, int windowIdx);
    void setup() override;
    void update() override;
    void draw() override;
    
private:
    DisplayManager* manager = nullptr;
    int windowIndex = -1;
    bool initialized = false;
};
