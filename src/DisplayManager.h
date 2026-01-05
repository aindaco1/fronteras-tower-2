#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class DisplayManager {
public:
    void setup();
    void update();
    void draw(int windowIndex);
    
    float getProximity() const { return proximity; }
    ofFbo& getFbo(int index) { return renderFbos[index]; }
    bool isSetup() const { return setupComplete; }
    
private:
    static const int NUM_OUTPUTS = 3;
    
    ofVideoGrabber webcam;
    ofxCvHaarFinder faceFinder;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg;
    
    vector<ofVideoPlayer> videos;
    vector<ofVec2f> videoLetterboxDims;  // Pre-calculated letterbox dims {drawW, drawY}
    ofImage staticImage;
    vector<ofTexture> staticImageTextures;  // One per window
    
    float proximity;
    int windowAssignment[NUM_OUTPUTS];
    
    float lastSwapTime;
    float swapInterval;

    int currentVideoIndex;
    
    // Static image mirror behavior
    float staticImageShowTime;
    bool inMirrorMode;
    float mirrorModeStartTime;
    float mirrorModeDuration;
    int mirrorSource;
    int lastStaticImageWindow;
    
    int frameSkip;
    int frameCounter;
    
    // False positive filtering
    int consecutiveDetections;
    int detectionThreshold;
    
    bool setupComplete;
    
    void updateProximity();
    void calculateLetterboxDims(int videoIndex);
    void reloadVideo(int videoIndex);
    
    vector<ofShader> glitchShaders; // One per window (GL context)
    vector<ofFbo> renderFbos; // One per window
    vector<ofTexture> webcamTextures; // One per window
    vector<ofTexture> videoTextures; // One per window
    
    // Track which frame each window last copied (for multi-context video sync)
    int videoFrameNumber;
    vector<int> lastCopiedVideoFrame; // One per window
    
    // Cached video pixels (avoid GL context issues with AVFoundation)
    ofPixels cachedVideoPixels;
    bool hasValidVideoPixels;
};
