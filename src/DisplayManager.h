#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class DisplayManager {
public:
    void setup();
    void update();
    void draw(int windowIndex);
    
    float getProximity() const { return proximity; }
    ofFbo& getFbo(int index) { return fbos[index]; }
    bool isSetup() const { return setupComplete; }
    
    void swapAssignments();
    
private:
    static const int NUM_OUTPUTS = 3;
    
    ofVideoGrabber webcam;
    ofxCvHaarFinder faceFinder;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg;
    
    vector<ofVideoPlayer> videos;
    vector<ofFbo> fbos;
    
    float proximity;
    int windowAssignment[NUM_OUTPUTS];
    
    float lastSwapTime;
    float swapInterval;
    
    int frameSkip;
    int frameCounter;
    
    // False positive filtering
    int consecutiveDetections;
    int detectionThreshold;
    
    bool setupComplete;
    
    void updateProximity();
    
    vector<ofShader> glitchShaders; // One per window (GL context)
    vector<ofFbo> renderFbos; // One per window
    vector<ofTexture> webcamTextures; // One per window
};
