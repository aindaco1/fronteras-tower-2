#include "DisplayManager.h"

void DisplayManager::setup() {
    ofLogNotice() << "DisplayManager::setup() - Starting";
    
    proximity = 0.0f;
    
    webcam.setup(640, 480);
    ofLogNotice() << "Webcam setup complete";
    
    // Try alternative cascade that sometimes works better
    string cascadeFile = "haarcascade_frontalface_alt2.xml";
    
    if(!ofFile::doesFileExist(cascadeFile)) {
        ofLogWarning() << cascadeFile << " not found, trying default...";
        cascadeFile = "haarcascade_frontalface_default.xml";
    }
    
    ofLogNotice() << "Loading cascade: " << cascadeFile;
    faceFinder.setup(cascadeFile);
    faceFinder.setScaleHaar(1.1f);
    faceFinder.setNeighbors(4);
    ofLogNotice() << "Face finder setup complete";
    
    colorImg.allocate(webcam.getWidth(), webcam.getHeight());
    grayImg.allocate(webcam.getWidth(), webcam.getHeight());
    ofLogNotice() << "Face detection setup complete";
}

void DisplayManager::update() {
    webcam.update();
    
    if(webcam.isFrameNew()) {
        // Properly convert to grayscale
        colorImg.setFromPixels(webcam.getPixels());
        grayImg.setFromColorImage(colorImg);  // Explicit conversion
        grayImg.blur(3);  // Reduce noise
        
        // Use haar detection - size range relative to frame
        int minDim = std::min(webcam.getWidth(), webcam.getHeight());
        int minSize = int(minDim * 0.18f);  // ~86px for 640x480
        int maxSize = int(minDim * 0.65f);  // ~312px for 640x480
        faceFinder.findHaarObjects(grayImg, minSize, minSize, maxSize, maxSize);
        
        // Debug output every 60 frames
        if(ofGetFrameNum() % 60 == 0) {
            ofLogNotice() << "Raw detections: " << faceFinder.blobs.size();
            for(int i = 0; i < faceFinder.blobs.size(); i++) {
                ofLogNotice() << "  Blob " << i << " size: " << faceFinder.blobs[i].boundingRect.width 
                             << "x" << faceFinder.blobs[i].boundingRect.height;
            }
        }
        
        updateProximity();
    }
}

void DisplayManager::draw(int windowIndex) {
    ofBackground(0);
    
    // Draw webcam fullscreen
    ofSetColor(255);
    webcam.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    // Calculate scale for overlays
    float sx = (float)ofGetWidth() / webcam.getWidth();
    float sy = (float)ofGetHeight() / webcam.getHeight();
    
    // Draw face detection rectangles
    ofSetLineWidth(2);
    for(int i = 0; i < faceFinder.blobs.size(); i++) {
        auto& rect = faceFinder.blobs[i].boundingRect;
        
        // Filter: keep near-square faces only
        float aspect = (float)rect.width / rect.height;
        bool validAspect = (aspect >= 0.75f && aspect <= 1.3f);
        
        if(!validAspect) continue;  // Skip invalid detections
        
        ofSetColor(0, 255, 0);
        ofNoFill();
        ofPushMatrix();
        ofScale(sx, sy);
        ofDrawRectangle(rect);
        ofPopMatrix();
        ofFill();
    }
    ofSetLineWidth(1);
    
    // Draw proximity info
    ofSetColor(255);
    ofDrawBitmapString("Proximity: " + ofToString(proximity, 2), 20, 20);
    ofDrawBitmapString("Faces detected: " + ofToString(faceFinder.blobs.size()), 20, 40);
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
