#include "DisplayManager.h"

void DisplayManager::setup() {
    ofLogNotice() << "DisplayManager::setup() - Starting";
    
    proximity = 0.0f;
    
    webcam.setup(640, 480);
    ofLogNotice() << "Webcam setup complete";
    
    faceFinder.setup("haarcascade_frontalface_default.xml");
    
    colorImg.allocate(webcam.getWidth(), webcam.getHeight());
    grayImg.allocate(webcam.getWidth(), webcam.getHeight());
    ofLogNotice() << "Face detection setup complete";
}

void DisplayManager::update() {
    webcam.update();
    
    if(webcam.isFrameNew()) {
        colorImg.setFromPixels(webcam.getPixels());
        grayImg = colorImg;
        
        // Parameters: image, scaleHaar, minNeighbors, flags, minWidth, minHeight
        faceFinder.findHaarObjects(grayImg, 80, 250);  // min 80px, max 250px face size
        updateProximity();
    }
}

void DisplayManager::draw(int windowIndex) {
    ofBackground(0);
    ofSetColor(255);
    webcam.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    // Draw face detection rectangles
    ofSetColor(0, 255, 0);
    for(int i = 0; i < faceFinder.blobs.size(); i++) {
        ofNoFill();
        ofDrawRectangle(faceFinder.blobs[i].boundingRect);
        ofFill();
    }
    
    // Draw proximity info
    ofSetColor(255);
    ofDrawBitmapString("Proximity: " + ofToString(proximity, 2), 20, 20);
    ofDrawBitmapString("Faces: " + ofToString(faceFinder.blobs.size()), 20, 40);
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
