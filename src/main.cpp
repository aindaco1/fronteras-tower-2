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

// Set to false to force windowed mode even with 3+ monitors
const bool FORCE_WINDOWED = false;

int main() {
    // Performance settings
    ofSetLogLevel(OF_LOG_NOTICE);  // Reduce log overhead
    
    globalManager = make_shared<DisplayManager>();
    
    ofGLFWWindowSettings settings;
    settings.setSize(720, 480);
    settings.resizable = true;
    settings.numSamples = 0;  // Disable antialiasing for performance
    settings.doubleBuffering = true;
    
    // Create first window (this initializes GLFW)
    settings.setPosition(ofVec2f(50, 50));
    auto window1 = ofCreateWindow(settings);
    auto app1 = make_shared<DisplayApp>();
    app1->init(globalManager.get(), 0);
    ofRunApp(window1, app1);
    
    // Now GLFW is initialized, we can query monitors
    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    
    ofLogNotice() << "Detected " << monitorCount << " monitor(s)";
    
    // Auto fullscreen when 3+ monitors detected (unless forced windowed)
    if (!FORCE_WINDOWED && monitorCount >= 3) {
        // Resize first window to fullscreen on monitor 0
        const GLFWvidmode* mode0 = glfwGetVideoMode(monitors[0]);
        int mx0, my0;
        glfwGetMonitorPos(monitors[0], &mx0, &my0);
        
        // Update window 1 to fullscreen
        glfwSetWindowAttrib(glfwGetCurrentContext(), GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowPos(glfwGetCurrentContext(), mx0, my0);
        glfwSetWindowSize(glfwGetCurrentContext(), mode0->width, mode0->height);
        
        ofLogNotice() << "Monitor 0: " << mode0->width << "x" << mode0->height << " at (" << mx0 << "," << my0 << ")";
        
        // Create window 2 fullscreen on monitor 1
        const GLFWvidmode* mode1 = glfwGetVideoMode(monitors[1]);
        int mx1, my1;
        glfwGetMonitorPos(monitors[1], &mx1, &my1);
        
        settings.decorated = false;
        settings.setSize(mode1->width, mode1->height);
        settings.setPosition(ofVec2f(mx1, my1));
        auto window2 = ofCreateWindow(settings);
        auto app2 = make_shared<DisplayApp>();
        app2->init(globalManager.get(), 1);
        ofRunApp(window2, app2);
        
        ofLogNotice() << "Monitor 1: " << mode1->width << "x" << mode1->height << " at (" << mx1 << "," << my1 << ")";
        
        // Create window 3 fullscreen on monitor 2
        const GLFWvidmode* mode2 = glfwGetVideoMode(monitors[2]);
        int mx2, my2;
        glfwGetMonitorPos(monitors[2], &mx2, &my2);
        
        settings.setSize(mode2->width, mode2->height);
        settings.setPosition(ofVec2f(mx2, my2));
        auto window3 = ofCreateWindow(settings);
        auto app3 = make_shared<DisplayApp>();
        app3->init(globalManager.get(), 2);
        ofRunApp(window3, app3);
        
        ofLogNotice() << "Monitor 2: " << mode2->width << "x" << mode2->height << " at (" << mx2 << "," << my2 << ")";
        ofLogNotice() << "Running in FULLSCREEN mode on 3 monitors";
        
    } else {
        // Windowed mode - create remaining windows
        settings.setPosition(ofVec2f(780, 50));
        auto window2 = ofCreateWindow(settings);
        auto app2 = make_shared<DisplayApp>();
        app2->init(globalManager.get(), 1);
        ofRunApp(window2, app2);

        settings.setPosition(ofVec2f(50, 560));
        auto window3 = ofCreateWindow(settings);
        auto app3 = make_shared<DisplayApp>();
        app3->init(globalManager.get(), 2);
        ofRunApp(window3, app3);
        
        ofLogNotice() << "Running in WINDOWED mode (" << monitorCount << " monitors detected)";
    }
    
    ofRunApp();
    
    return 0;
}
