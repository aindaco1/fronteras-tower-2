#include "ofMain.h"
#include "DisplayApp.h"
#include "DisplayManager.h"

shared_ptr<DisplayManager> globalManager;

int main() {
    globalManager = make_shared<DisplayManager>();
    
    ofGLFWWindowSettings settings;
    settings.setSize(720, 480);
    settings.resizable = true;
    
    // Window 1 - Top Left (with title bar offset)
    settings.setPosition(ofVec2f(50, 50));
    auto window1 = ofCreateWindow(settings);
    auto app1 = make_shared<DisplayApp>();
    app1->init(globalManager.get(), 0);
    ofRunApp(window1, app1);

    // Window 2 - Top Right
    settings.setPosition(ofVec2f(750, 50));
    auto window2 = ofCreateWindow(settings);
    auto app2 = make_shared<DisplayApp>();
    app2->init(globalManager.get(), 1);
    ofRunApp(window2, app2);

    // Window 3 - Bottom Left
    settings.setPosition(ofVec2f(50, 530));
    auto window3 = ofCreateWindow(settings);
    auto app3 = make_shared<DisplayApp>();
    app3->init(globalManager.get(), 2);
    ofRunApp(window3, app3);
    
    ofRunApp();
    
    return 0;
}
