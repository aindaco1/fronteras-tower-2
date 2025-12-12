#include "ofMain.h"
#include "DisplayApp.h"
#include "DisplayManager.h"
#include "GLFW/glfw3.h"

// Force dedicated GPU on Windows (NVIDIA Optimus / AMD PowerXpress)
#ifdef _WIN32
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

shared_ptr<DisplayManager> globalManager;

int main() {
    // Initialize GLFW before querying monitors
    if (!glfwInit()) {
        ofLogError() << "Failed to initialize GLFW";
        return -1;
    }
    
    globalManager = make_shared<DisplayManager>();
    
    // Check how many monitors are connected
    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    
    if (monitors == nullptr) {
        monitorCount = 0;
    }
    
    bool useFullscreen = (monitorCount >= 3);
    
    ofLogNotice() << "Detected " << monitorCount << " monitor(s). Fullscreen mode: " << (useFullscreen ? "ON" : "OFF");
    
    ofGLFWWindowSettings settings;
    
    if (useFullscreen && monitorCount >= 3) {
        // Fullscreen mode on 3 separate monitors
        for (int i = 0; i < 3; i++) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            
            settings.setSize(mode->width, mode->height);
            settings.resizable = false;
            settings.decorated = false;  // No title bar
            settings.monitor = i;  // Assign to specific monitor
            settings.windowMode = OF_FULLSCREEN;
            
            int mx, my;
            glfwGetMonitorPos(monitors[i], &mx, &my);
            settings.setPosition(ofVec2f(mx, my));
            
            auto window = ofCreateWindow(settings);
            auto app = make_shared<DisplayApp>();
            app->init(globalManager.get(), i);
            ofRunApp(window, app);
            
            ofLogNotice() << "Window " << i << " fullscreen on monitor " << i 
                          << " (" << mode->width << "x" << mode->height << ")";
        }
    } else {
        // Windowed mode (for development or fewer monitors)
        settings.setSize(720, 480);
        settings.resizable = true;
        
        // Window 1 - Top Left
        settings.setPosition(ofVec2f(50, 50));
        auto window1 = ofCreateWindow(settings);
        auto app1 = make_shared<DisplayApp>();
        app1->init(globalManager.get(), 0);
        ofRunApp(window1, app1);

        // Window 2 - Top Right
        settings.setPosition(ofVec2f(780, 50));
        auto window2 = ofCreateWindow(settings);
        auto app2 = make_shared<DisplayApp>();
        app2->init(globalManager.get(), 1);
        ofRunApp(window2, app2);

        // Window 3 - Bottom Left
        settings.setPosition(ofVec2f(50, 560));
        auto window3 = ofCreateWindow(settings);
        auto app3 = make_shared<DisplayApp>();
        app3->init(globalManager.get(), 2);
        ofRunApp(window3, app3);
    }
    
    ofRunApp();
    
    return 0;
}
