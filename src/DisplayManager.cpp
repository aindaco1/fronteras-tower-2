#include "DisplayManager.h"

void DisplayManager::setup() {
    ofLogNotice() << "DisplayManager::setup() - Starting";
    
    setupComplete = false;
    proximity = 0.0f;
    frameSkip = 2;  // Process every 3rd frame (0,1,skip,0,1,skip...)
    frameCounter = 0;
    consecutiveDetections = 0;
    detectionThreshold = 3;  // Must detect face in 3+ consecutive processed frames
    
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
    
    // Optimized for low-res cameras and edge detection
    faceFinder.setScaleHaar(1.2f);  // Faster, still accurate (was 1.1)
    faceFinder.setNeighbors(2);     // Balanced sensitivity (2 = good for low-res + reduces false positives)
    
    ofLogNotice() << "Face finder setup complete";
    
    colorImg.allocate(webcam.getWidth(), webcam.getHeight());
    grayImg.allocate(webcam.getWidth(), webcam.getHeight());
    ofLogNotice() << "Face detection setup complete";
    
    // Allocate vectors for 3 windows (shaders loaded per-window in draw)
    renderFbos.resize(NUM_OUTPUTS);
    glitchShaders.resize(NUM_OUTPUTS);
    webcamTextures.resize(NUM_OUTPUTS);
    
    setupComplete = true;
    ofLogNotice() << "DisplayManager setup complete!";
}

void DisplayManager::update() {
    webcam.update();
    
    if(webcam.isFrameNew()) {
        // Frame skipping for performance
        frameCounter++;
        if(frameCounter % (frameSkip + 1) != 0) {
            return;  // Skip this frame
        }
        
        // Properly convert to grayscale
        colorImg.setFromPixels(webcam.getPixels());
        grayImg.setFromColorImage(colorImg);  // Explicit conversion
        
        // Use haar detection - size range relative to frame
        int minDim = std::min(webcam.getWidth(), webcam.getHeight());
        int minSize = int(minDim * 0.20f);  // ~96px for 640x480 (filter small false positives)
        int maxSize = int(minDim * 0.95f);  // ~456px for 640x480 (allow very close faces)
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
    
    // Load shader for this window's GL context (only once)
    if(!glitchShaders[windowIndex].isLoaded()) {
        if(glitchShaders[windowIndex].load("shaders/glitch")) {
            ofLogNotice() << "Loaded shader for window " << windowIndex;
        }
    }
    
    // Allocate FBO for this window if needed
    if(!renderFbos[windowIndex].isAllocated() || 
       renderFbos[windowIndex].getWidth() != ofGetWidth() || 
       renderFbos[windowIndex].getHeight() != ofGetHeight()) {
        renderFbos[windowIndex].allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
        ofLogNotice() << "Allocated FBO for window " << windowIndex;
    }
    
    // Update webcam texture for this GL context
    if(webcam.isInitialized() && webcam.getPixels().size() > 0) {
        webcamTextures[windowIndex].loadData(webcam.getPixels());
    }
    
    // Draw to FBO
    renderFbos[windowIndex].begin();
    ofClear(0, 0, 0, 255);
    
    // Draw webcam
    ofSetColor(255);
    webcamTextures[windowIndex].draw(0, 0, renderFbos[windowIndex].getWidth(), renderFbos[windowIndex].getHeight());
    
    // Calculate scale for overlays
    float sx = (float)renderFbos[windowIndex].getWidth() / webcam.getWidth();
    float sy = (float)renderFbos[windowIndex].getHeight() / webcam.getHeight();
    
    // Draw face detection rectangles
    ofSetLineWidth(2);
    int minDim = std::min(webcam.getWidth(), webcam.getHeight());
    float minAllowedSize = minDim * 0.20f;
    
    for(int i = 0; i < faceFinder.blobs.size(); i++) {
        auto& rect = faceFinder.blobs[i].boundingRect;
        
        // Filter: size check
        if(rect.width < minAllowedSize) continue;
        
        // Filter: aspect ratio for partial/angled faces
        float aspect = (float)rect.width / rect.height;
        bool validAspect = (aspect >= 0.65f && aspect <= 1.55f);
        
        if(!validAspect) continue;
        
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
    
    // Show face size for debugging
    if(faceFinder.blobs.size() > 0) {
        float faceSize = faceFinder.blobs[0].boundingRect.width;
        ofDrawBitmapString("Face size: " + ofToString(faceSize, 0) + "px", 20, 60);
    }
    
    renderFbos[windowIndex].end();
    
    // Apply glitch shader
    ofSetColor(255);
    if(glitchShaders[windowIndex].isLoaded()) {
        float glitchIntensity = proximity * 2.0f;
        
        glitchShaders[windowIndex].begin();
        glitchShaders[windowIndex].setUniformTexture("tex0", renderFbos[windowIndex].getTexture(), 0);
        glitchShaders[windowIndex].setUniform1f("intensity", glitchIntensity);
        glitchShaders[windowIndex].setUniform1f("time", ofGetElapsedTimef());
        renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
        glitchShaders[windowIndex].end();
    } else {
        renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
    }
}

void DisplayManager::updateProximity() {
    float targetProximity = proximity;
    
    if(faceFinder.blobs.size() > 0) {
        consecutiveDetections++;
        
        // Only update proximity if we've seen face consistently
        if(consecutiveDetections >= detectionThreshold) {
            // Use largest valid detection (most likely real face)
            int minDim = std::min(webcam.getWidth(), webcam.getHeight());
            float minAllowedSize = minDim * 0.20f;
            float largestFaceSize = 0;
            
            for(int i = 0; i < faceFinder.blobs.size(); i++) {
                auto& rect = faceFinder.blobs[i].boundingRect;
                
                // Apply same filters as drawing
                if(rect.width < minAllowedSize) continue;
                float aspect = (float)rect.width / rect.height;
                if(aspect < 0.65f || aspect > 1.55f) continue;
                
                largestFaceSize = std::max(largestFaceSize, (float)rect.width);
            }
            
            // Only update if we found a valid face
            if(largestFaceSize > 0) {
                float minDetectionSize = minDim * 0.20f;
                float maxDetectionSize = minDim * 0.95f;
                targetProximity = ofMap(largestFaceSize, minDetectionSize, maxDetectionSize, 0.0f, 1.0f, true);
            }
        }
    } else {
        consecutiveDetections = 0;  // Reset counter
        targetProximity *= 0.92f;  // Decay slightly faster
    }
    
    // Smooth proximity changes to reduce jitter
    proximity = ofLerp(proximity, targetProximity, 0.15f);
}

void DisplayManager::swapAssignments() {
    int temp = windowAssignment[0];
    windowAssignment[0] = windowAssignment[1];
    windowAssignment[1] = windowAssignment[2];
    windowAssignment[2] = temp;
}


