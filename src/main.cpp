#include "ofMain.h"
#include "DisplayApp.h"
#include "DisplayManager.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(1920, 1080);
    settings.windowMode = OF_WINDOW;
    
    DisplayManager manager;
    manager.setup();
    
    vector<shared_ptr<ofAppBaseWindow>> windows;
    vector<shared_ptr<DisplayApp>> apps;
    
    for(int i = 0; i < 3; i++) {
        settings.setPosition(ofVec2f(i * 1920, 0));
        auto window = ofCreateWindow(settings);
        windows.push_back(window);
        
        auto app = make_shared<DisplayApp>();
        app->setup(&manager, i);
        apps.push_back(app);
        
        ofRunApp(window, app);
    }
    
    ofRunMainLoop();
    return 0;
}
