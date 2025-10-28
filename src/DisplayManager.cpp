#include "DisplayManager.h"

void DisplayManager::setup() {
    proximity = 0.0f;
    swapInterval = 10.0f;
    lastSwapTime = ofGetElapsedTimef();
    
    webcam.setup(1920, 1080);
    
    faceFinder.setup("haarcascade_frontalface_default.xml");
    colorImg.allocate(webcam.getWidth(), webcam.getHeight());
    grayImg.allocate(webcam.getWidth(), webcam.getHeight());
    
    for(int i = 0; i < NUM_OUTPUTS; i++) {
        ofFbo fbo;
        fbo.allocate(1920, 1080, GL_RGBA);
        fbos.push_back(fbo);
        windowAssignment[i] = i;
    }
    
    ofDirectory dir("movies");
    dir.allowExt("mp4");
    dir.allowExt("mov");
    dir.listDir();
    
    for(int i = 0; i < dir.size(); i++) {
        ofVideoPlayer video;
        video.load(dir.getPath(i));
        video.setLoopState(OF_LOOP_NORMAL);
        video.play();
        videos.push_back(video);
    }
    
    proximityShader.load("shaders/proximity");
}

void DisplayManager::update() {
    webcam.update();
    
    if(webcam.isFrameNew()) {
        colorImg.setFromPixels(webcam.getPixels());
        grayImg = colorImg;
        faceFinder.findHaarObjects(grayImg);
        updateProximity();
    }
    
    for(auto& video : videos) {
        video.update();
    }
    
    if(ofGetElapsedTimef() - lastSwapTime > swapInterval) {
        swapAssignments();
        lastSwapTime = ofGetElapsedTimef();
    }
    
    for(int i = 0; i < NUM_OUTPUTS; i++) {
        fbos[i].begin();
        ofClear(0, 0, 0);
        
        int assignedSource = windowAssignment[i];
        if(assignedSource == 0) {
            webcam.draw(0, 0, 1920, 1080);
        } else if(assignedSource - 1 < videos.size()) {
            videos[assignedSource - 1].draw(0, 0, 1920, 1080);
        }
        
        fbos[i].end();
        
        applyShaders(fbos[i], proximity);
    }
}

void DisplayManager::draw(int windowIndex) {
    if(windowIndex < NUM_OUTPUTS) {
        fbos[windowIndex].draw(0, 0);
    }
}

void DisplayManager::updateProximity() {
    if(faceFinder.blobs.size() > 0) {
        float faceSize = faceFinder.blobs[0].boundingRect.width;
        float maxSize = webcam.getWidth() * 0.5f;
        proximity = ofClamp(faceSize / maxSize, 0.0f, 1.0f);
    } else {
        proximity *= 0.95f;
    }
}

void DisplayManager::swapAssignments() {
    int temp = windowAssignment[0];
    windowAssignment[0] = windowAssignment[1];
    windowAssignment[1] = windowAssignment[2];
    windowAssignment[2] = temp;
}

void DisplayManager::applyShaders(ofFbo& fbo, float intensity) {
    if(proximityShader.isLoaded()) {
        fbo.begin();
        proximityShader.begin();
        proximityShader.setUniform1f("intensity", intensity);
        proximityShader.setUniform1f("time", ofGetElapsedTimef());
        fbo.draw(0, 0);
        proximityShader.end();
        fbo.end();
    }
}
