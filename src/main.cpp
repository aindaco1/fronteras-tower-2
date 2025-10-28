#include "ofMain.h"
#include "DisplayApp.h"
#include "DisplayManager.h"

shared_ptr<DisplayManager> globalManager;

int main() {
    globalManager = make_shared<DisplayManager>();
    
    ofSetupOpenGL(1920, 1080, OF_WINDOW);
    
    auto app = make_shared<DisplayApp>();
    app->init(globalManager.get(), 0);
    ofRunApp(app);
    
    return 0;
}
