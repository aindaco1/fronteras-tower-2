#include "DisplayManager.h"

void DisplayManager::setup() {
	ofLogNotice() << "DisplayManager::setup() - Starting";

	setupComplete = false;
	proximity = 0.0f;
	frameSkip = 1; // Process every other frame for face detection
	frameCounter = 0;
	consecutiveDetections = 0;
	detectionThreshold = 3; // Must detect face in 3+ consecutive processed frames
	
	staticImageShowTime = ofGetElapsedTimef();
	inMirrorMode = false;
	mirrorModeStartTime = 0;
	mirrorModeDuration = 0;
	mirrorSource = 0;
	lastStaticImageWindow = 2;

	// Reduced resolution for better performance
	webcam.setDesiredFrameRate(30);  // Limit webcam framerate
	webcam.setup(320, 240);
	ofLogNotice() << "Webcam setup complete";

	// Try alternative cascade that sometimes works better
	string cascadeFile = "haarcascade_frontalface_alt2.xml";

	if (!ofFile::doesFileExist(cascadeFile)) {
		ofLogWarning() << cascadeFile << " not found, trying default...";
		cascadeFile = "haarcascade_frontalface_default.xml";
	}

	ofLogNotice() << "Loading cascade: " << cascadeFile;
	faceFinder.setup(cascadeFile);

	// Optimized for low-res cameras and edge detection
	faceFinder.setScaleHaar(1.2f); // Faster, still accurate (was 1.1)
	faceFinder.setNeighbors(2); // Balanced sensitivity (2 = good for low-res + reduces false positives)

	ofLogNotice() << "Face finder setup complete";

	colorImg.allocate(320, 240);
	grayImg.allocate(320, 240);
	ofLogNotice() << "Face detection setup complete";

	// Allocate vectors for 3 windows (shaders loaded per-window in draw)
	renderFbos.resize(NUM_OUTPUTS);
	glitchShaders.resize(NUM_OUTPUTS);
	webcamTextures.resize(NUM_OUTPUTS);
	videoTextures.resize(NUM_OUTPUTS);
	staticImageTextures.resize(NUM_OUTPUTS);

	// Load videos from data/movies/
	ofDirectory dir("movies/");
	dir.allowExt("mov");
	dir.allowExt("mp4");
	dir.allowExt("avi");
	dir.listDir();

	for (auto & file : dir) {
		ofVideoPlayer player;
		if (player.load(file.getAbsolutePath())) {
			player.setLoopState(OF_LOOP_NONE); // Play once, no loop
			videos.push_back(player);
			int videoIndex = videos.size() - 1;
			calculateLetterboxDims(videoIndex);
			ofLogNotice() << "Loaded video: " << file.getFileName() 
				<< " (" << player.getWidth() << "x" << player.getHeight() << ")";
		}
	}

	ofLogNotice() << "Loaded " << videos.size() << " videos";

	currentVideoIndex = 0;
	if (videos.size() > 0) {
		videos[0].play();
	}

	// Load static image for window 2
	if (ofFile::doesFileExist("images/test.jpg")) {
		staticImage.load("images/test.jpg");
		ofLogNotice() << "Loaded test.jpg: " << staticImage.getWidth() << "x" << staticImage.getHeight() 
			<< " (allocated: " << staticImage.isAllocated() << ")";
	} else if (ofFile::doesFileExist("images/static.png")) {
		staticImage.load("images/static.png");
		ofLogNotice() << "Loaded static.png: " << staticImage.getWidth() << "x" << staticImage.getHeight() 
			<< " (allocated: " << staticImage.isAllocated() << ")";
	} else {
		ofLogWarning() << "No static image found in images/ folder";
	}



	// Initialize assignments: 0=webcam, 1=current video, 2=static image
	windowAssignment[0] = 0;
	windowAssignment[1] = 1;
	windowAssignment[2] = 2;

	swapInterval = ofRandom(1.0f, 30.0f); // random 1-30 seconds
	lastSwapTime = ofGetElapsedTimef();

	setupComplete = true;
	ofLogNotice() << "DisplayManager setup complete!";
}

void DisplayManager::update() {
	webcam.update();

	// Update all videos
	for (auto & video : videos) {
		video.update();
	}

	// Check for video end and switch to next
	if (videos.size() > 0) {
		ofVideoPlayer& currentVideo = videos[currentVideoIndex];
		// Check if video reached the end (current frame >= total frames or video stopped)
		if (currentVideo.getTotalNumFrames() > 0 && 
		    (currentVideo.getCurrentFrame() >= currentVideo.getTotalNumFrames() - 1 || !currentVideo.isPlaying())) {
			currentVideoIndex = (currentVideoIndex + 1) % videos.size();
			reloadVideo(currentVideoIndex);
		}
	}

	if (webcam.isFrameNew()) {
		// Frame skipping for performance
		frameCounter++;
		if (frameCounter % (frameSkip + 1) != 0) {
			return; // Skip this frame
		}

		// Ensure images match webcam size (safety check)
		if (colorImg.width != webcam.getWidth() || colorImg.height != webcam.getHeight()) {
			colorImg.allocate(webcam.getWidth(), webcam.getHeight());
			grayImg.allocate(webcam.getWidth(), webcam.getHeight());
			ofLogNotice() << "Reallocated CV images to match webcam: " 
						  << webcam.getWidth() << "x" << webcam.getHeight();
		}

		// Properly convert to grayscale
		colorImg.setFromPixels(webcam.getPixels());
		grayImg.setFromColorImage(colorImg); // Explicit conversion

		// Use haar detection - size range relative to frame
		int minDim = std::min(webcam.getWidth(), webcam.getHeight());
		int minSize = int(minDim * 0.20f); // ~96px for 640x480 (filter small false positives)
		int maxSize = int(minDim * 0.95f); // ~456px for 640x480 (allow very close faces)
		faceFinder.findHaarObjects(grayImg, minSize, minSize, maxSize, maxSize);

		// Debug output every 60 frames
		if (ofGetFrameNum() % 60 == 0) {
			ofLogNotice() << "Raw detections: " << faceFinder.blobs.size();
			for (int i = 0; i < faceFinder.blobs.size(); i++) {
				ofLogNotice() << "  Blob " << i << " size: " << faceFinder.blobs[i].boundingRect.width
							  << "x" << faceFinder.blobs[i].boundingRect.height;
			}
		}

		updateProximity();
	}

	// Check for static image mirror behavior
	if (videos.size() > 0) {
		// Find which window currently has the static image
		int currentStaticImageWindow = -1;
		for (int i = 0; i < NUM_OUTPUTS; i++) {
			if (windowAssignment[i] == 2) {
				currentStaticImageWindow = i;
				break;
			}
		}
		
		// Only run behavior if static image exists and window changed, reset timer
		if (currentStaticImageWindow != lastStaticImageWindow) {
			lastStaticImageWindow = currentStaticImageWindow;
			staticImageShowTime = ofGetElapsedTimef();
			inMirrorMode = false;
		}
		
		if (currentStaticImageWindow >= 0) {
			float elapsedSinceImageShow = ofGetElapsedTimef() - staticImageShowTime;
			
			if (inMirrorMode) {
				// Check if mirror mode should end
				if (ofGetElapsedTimef() - mirrorModeStartTime > mirrorModeDuration) {
					// Revert to static image
					windowAssignment[currentStaticImageWindow] = 2;
					inMirrorMode = false;
					staticImageShowTime = ofGetElapsedTimef();
					ofLogNotice() << "Window " << currentStaticImageWindow << " reverted to JPEG";
				}
			} else {
				// Check if we should start mirror mode
				if (elapsedSinceImageShow > 5.0f) {
					// Start mirroring either webcam (0) or video (1)
					mirrorSource = ofRandom(0, 2);
					windowAssignment[currentStaticImageWindow] = mirrorSource;
					inMirrorMode = true;
					mirrorModeStartTime = ofGetElapsedTimef();
					mirrorModeDuration = ofRandom(5.0f, 10.0f);
					ofLogNotice() << "Window " << currentStaticImageWindow << " mirroring source " << mirrorSource 
						<< " for " << mirrorModeDuration << " seconds";
				}
			}
		}
	}

	// Check for time-based swapping
	if (ofGetElapsedTimef() - lastSwapTime > swapInterval) {
		// Randomly shuffle assignments among all 3 windows and 3 sources
		// Each source (0=webcam, 1=video, 2=static) goes to exactly one window
		vector<int> sources = {0, 1, 2};
		vector<int> oldAssignment(windowAssignment, windowAssignment + NUM_OUTPUTS);
		
		bool validShuffle = false;
		int maxAttempts = 100;
		int attempts = 0;
		
		while (!validShuffle && attempts < maxAttempts) {
			attempts++;
			// Fisher-Yates shuffle
			for (int i = (int)sources.size() - 1; i > 0; i--) {
				int j = (int)ofRandom(0, i + 1);
				std::swap(sources[i], sources[j]);
			}
			
			// Verify no duplicates (should always be true with proper shuffle)
			bool hasDuplicates = (sources[0] == sources[1] || sources[1] == sources[2] || sources[0] == sources[2]);
			
			// Check if at least 2 positions changed
			int changes = 0;
			for (int i = 0; i < NUM_OUTPUTS; i++) {
				if (sources[i] != oldAssignment[i]) {
					changes++;
				}
			}
			
			if (!hasDuplicates && changes >= 2) {
				validShuffle = true;
			}
		}
		
		// Only apply if we got a valid shuffle
		if (validShuffle) {
			for (int i = 0; i < NUM_OUTPUTS; i++) {
				windowAssignment[i] = sources[i];
			}
			ofLogNotice() << "Swapped assignments: window 0=" << windowAssignment[0] 
				<< ", window 1=" << windowAssignment[1] 
				<< ", window 2=" << windowAssignment[2];
		}
		
		lastSwapTime = ofGetElapsedTimef();
		swapInterval = ofRandom(1.0f, 30.0f);
	}
}

void DisplayManager::draw(int windowIndex) {
	ofBackground(0);

	// Load shader for this window's GL context (only once)
	if (!glitchShaders[windowIndex].isLoaded()) {
		if (glitchShaders[windowIndex].load("shaders/glitch")) {
			ofLogNotice() << "Loaded shader for window " << windowIndex;
		}
	}

	// Allocate FBO for this window if needed
	if (!renderFbos[windowIndex].isAllocated() || renderFbos[windowIndex].getWidth() != ofGetWidth() || renderFbos[windowIndex].getHeight() != ofGetHeight()) {
		renderFbos[windowIndex].allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
		ofLogNotice() << "Allocated FBO for window " << windowIndex << ": " << ofGetWidth() << "x" << ofGetHeight();
	}

	// Update webcam texture for this GL context only when new frame
	if (webcam.isInitialized() && webcam.isFrameNew() && webcam.getPixels().size() > 0) {
		webcamTextures[windowIndex].loadData(webcam.getPixels());
	}

	// Draw to FBO
	renderFbos[windowIndex].begin();
	ofClear(0, 0, 0, 255);

	// Draw assigned content: 0=webcam, 1=video, 2=static image
	ofSetColor(255);
	int assignment = windowAssignment[windowIndex];

	if (assignment == 0) {
		// Draw webcam fullscreen (no letterboxing)
		webcamTextures[windowIndex].draw(0, 0, renderFbos[windowIndex].getWidth(), renderFbos[windowIndex].getHeight());
	} else if (assignment == 1) {
		// Draw video fullscreen to FBO (letterboxing will be applied at screen render time)
		if (videos.size() > 0 && currentVideoIndex < videos.size()) {
			// Update texture if video frame is new
			if (videos[currentVideoIndex].isFrameNew() && videos[currentVideoIndex].getPixels().size() > 0) {
				videoTextures[windowIndex].loadData(videos[currentVideoIndex].getPixels());
			}
			// Draw fullscreen to FBO
			videoTextures[windowIndex].draw(0, 0, renderFbos[windowIndex].getWidth(), renderFbos[windowIndex].getHeight());
		}
	} else if (assignment == 2) {
		// Draw static image fullscreen to FBO
		// Load texture on-demand in this GL context
		if (staticImage.isAllocated() && !staticImageTextures[windowIndex].isAllocated()) {
			staticImageTextures[windowIndex].loadData(staticImage.getPixels());
			ofLogNotice() << "Window " << windowIndex << " - loaded static image texture: " 
				<< staticImageTextures[windowIndex].getWidth() << "x" << staticImageTextures[windowIndex].getHeight();
		}
		
		if (staticImageTextures[windowIndex].isAllocated()) {
			staticImageTextures[windowIndex].draw(0, 0, renderFbos[windowIndex].getWidth(), renderFbos[windowIndex].getHeight());
		}
	}

	// Draw overlays only for webcam
	if (assignment == 0) {
		// Calculate scale for overlays
		float sx = (float)renderFbos[windowIndex].getWidth() / webcam.getWidth();
		float sy = (float)renderFbos[windowIndex].getHeight() / webcam.getHeight();

		// Draw face detection rectangles
		ofSetLineWidth(2);
		int minDim = std::min(webcam.getWidth(), webcam.getHeight());
		float minAllowedSize = minDim * 0.20f;

		for (int i = 0; i < faceFinder.blobs.size(); i++) {
			auto & rect = faceFinder.blobs[i].boundingRect;

			// Filter: size check
			if (rect.width < minAllowedSize) continue;

			// Filter: aspect ratio for partial/angled faces
			float aspect = (float)rect.width / rect.height;
			bool validAspect = (aspect >= 0.65f && aspect <= 1.55f);

			if (!validAspect) continue;

			ofSetColor(0, 255, 0);
			ofNoFill();
			ofPushMatrix();
			ofScale(sx, sy);
			ofDrawRectangle(rect);
			ofPopMatrix();
			ofFill();
		}
		ofSetLineWidth(1);
	}

	renderFbos[windowIndex].end();

	// Draw debug info only on webcam window
	if (assignment == 0) {
		ofSetColor(255);
		ofDrawBitmapString("Proximity: " + ofToString(proximity, 2), 20, 20);
		ofDrawBitmapString("Faces detected: " + ofToString(faceFinder.blobs.size()), 20, 40);

		// Show face size for debugging
		if (faceFinder.blobs.size() > 0) {
			float faceSize = faceFinder.blobs[0].boundingRect.width;
			ofDrawBitmapString("Face size: " + ofToString(faceSize, 0) + "px", 20, 60);
		}
	}

	// Apply glitch shader only to webcam
	ofSetColor(255);
	if (assignment == 0 && glitchShaders[windowIndex].isLoaded()) {
		float glitchIntensity = proximity * 2.0f;

		glitchShaders[windowIndex].begin();
		glitchShaders[windowIndex].setUniformTexture("tex0", renderFbos[windowIndex].getTexture(), 0);
		glitchShaders[windowIndex].setUniform1f("intensity", glitchIntensity);
		glitchShaders[windowIndex].setUniform1f("time", ofGetElapsedTimef());
		renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
		glitchShaders[windowIndex].end();
	} else if (assignment == 1) {
		// Draw video letterboxed to fit window
		if (videos.size() > 0 && currentVideoIndex >= 0 && currentVideoIndex < videos.size()) {
			float videoW = videos[currentVideoIndex].getWidth();
			float videoH = videos[currentVideoIndex].getHeight();
			float windowW = ofGetWidth();
			float windowH = ofGetHeight();
			
			if (videoW > 0 && videoH > 0) {
				float videoAspect = videoW / videoH;
				float windowAspect = windowW / windowH;
				float drawW, drawH, drawX, drawY;
				
				if (videoAspect > windowAspect) {
					// Video is wider - fit to width
					drawW = windowW;
					drawH = windowW / videoAspect;
					drawX = 0;
					drawY = (windowH - drawH) * 0.5f;
				} else {
					// Video is taller - fit to height
					drawH = windowH;
					drawW = windowH * videoAspect;
					drawX = (windowW - drawW) * 0.5f;
					drawY = 0;
				}
				
				renderFbos[windowIndex].draw(drawX, drawY, drawW, drawH);
			} else {
				renderFbos[windowIndex].draw(0, 0, windowW, windowH);
			}
		} else {
			renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
		}
	} else if (assignment == 2) {
		// Draw static image fullscreen (no letterboxing)
		renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
	} else {
		renderFbos[windowIndex].draw(0, 0, ofGetWidth(), ofGetHeight());
	}
}

void DisplayManager::updateProximity() {
	float targetProximity = proximity;

	if (faceFinder.blobs.size() > 0) {
		consecutiveDetections++;

		// Only update proximity if we've seen face consistently
		if (consecutiveDetections >= detectionThreshold) {
			// Use largest valid detection (most likely real face)
			int minDim = std::min(webcam.getWidth(), webcam.getHeight());
			float minAllowedSize = minDim * 0.20f;
			float largestFaceSize = 0;

			for (int i = 0; i < faceFinder.blobs.size(); i++) {
				auto & rect = faceFinder.blobs[i].boundingRect;

				// Apply same filters as drawing
				if (rect.width < minAllowedSize) continue;
				float aspect = (float)rect.width / rect.height;
				if (aspect < 0.65f || aspect > 1.55f) continue;

				largestFaceSize = std::max(largestFaceSize, (float)rect.width);
			}

			// Only update if we found a valid face
			if (largestFaceSize > 0) {
				float minDetectionSize = minDim * 0.20f;
				float maxDetectionSize = minDim * 0.95f;
				targetProximity = ofMap(largestFaceSize, minDetectionSize, maxDetectionSize, 0.0f, 1.0f, true);
			}
		}
	} else {
		consecutiveDetections = 0; // Reset counter
		targetProximity *= 0.92f; // Decay slightly faster
	}

	// Smooth proximity changes to reduce jitter
	proximity = ofLerp(proximity, targetProximity, 0.15f);
}

void DisplayManager::calculateLetterboxDims(int videoIndex) {
	if (videoIndex < 0 || videoIndex >= videos.size()) {
		return;
	}

	// Ensure vector is large enough
	if (videoIndex >= videoLetterboxDims.size()) {
		videoLetterboxDims.resize(videoIndex + 1, ofVec2f(0, 0));
	}

	float videoW = videos[videoIndex].getWidth();
	float videoH = videos[videoIndex].getHeight();

	if (videoW <= 0 || videoH <= 0) {
		ofLogWarning() << "Video " << videoIndex << " has invalid dimensions: " << videoW << "x" << videoH;
		videoLetterboxDims[videoIndex] = ofVec2f(0, 0);
		return;
	}

	// Target frame: 4:3 aspect ratio (720x540 for width=720)
	float frameW = 720.0f;
	float frameH = 540.0f; // 4:3 aspect ratio
	float frameAspect = frameW / frameH;

	float videoAspect = videoW / videoH;
	float drawW, drawH;

	// If video is wider than frame, scale to fill width
	if (videoAspect > frameAspect) {
		drawW = frameW;
		drawH = frameW / videoAspect;
	} else {
		// Otherwise scale to fill height
		drawH = frameH;
		drawW = frameH * videoAspect;
	}

	// Center vertically
	float drawY = (frameH - drawH) * 0.5f;

	videoLetterboxDims[videoIndex] = ofVec2f(drawW, drawY);

	ofLogNotice() << "Letterbox dims for video " << videoIndex 
		<< ": videoAspect=" << videoAspect 
		<< ", drawW=" << drawW 
		<< ", drawH=" << drawH 
		<< ", drawY=" << drawY;
}

void DisplayManager::reloadVideo(int videoIndex) {
	if (videoIndex < 0 || videoIndex >= videos.size()) {
		return;
	}

	ofVideoPlayer& video = videos[videoIndex];

	// Close and clear the video
	video.close();

	// Clear textures for all windows so they get reloaded
	for (int i = 0; i < videoTextures.size(); i++) {
		if (videoTextures[i].isAllocated()) {
			videoTextures[i].clear();
		}
	}

	// Reload from disk
	ofDirectory dir("movies/");
	dir.allowExt("mov");
	dir.allowExt("mp4");
	dir.allowExt("avi");
	dir.listDir();

	if (videoIndex < dir.size()) {
		video.load(dir.getPath(videoIndex));
		// Ensure dimensions are available after load
		int maxRetries = 10;
		int retries = 0;
		while ((video.getWidth() <= 0 || video.getHeight() <= 0) && retries < maxRetries) {
			ofSleepMillis(10);
			retries++;
		}

		// Recalculate letterbox dimensions for reloaded video
		calculateLetterboxDims(videoIndex);

		video.play();
		ofLogNotice() << "Reloaded video " << videoIndex 
			<< " - dims: " << video.getWidth() << "x" << video.getHeight();
	} else {
		ofLogWarning() << "Video index " << videoIndex << " exceeds directory size";
	}
}

