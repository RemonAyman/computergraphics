#ifndef APP_H
#define APP_H

#include <GL/freeglut.h>
#include "Common.h"
#include "Shape.h"
#include <vector>
#include <memory>

class App {
public:
    static App& getInstance() {
        static App instance;
        return instance;
    }

    void init(int argc, char** argv);
    void run();

    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);
    static void passiveMouseCallback(int x, int y);
    static void timerCallback(int value);
    static void menuCallback(int id);

private:
    App();
    void render();
    void renderGrid();
    void renderUI();
    void renderSidebar();
    void renderStatusBar();
    void drawText(int x, int y, std::string text, void* font = GLUT_BITMAP_HELVETICA_12);

    // Core State
    AppMode currentMode;
    Point mousePos;
    bool isDrawing;
    bool isSplitScreen;
    bool isAnimating;
    float animAngle;
    float animScale;
    bool scaleUp;

    std::vector<Point> inputPoints;
    std::vector<std::unique_ptr<Shape>> shapes;
    std::vector<std::unique_ptr<Shape>> redoStack;
    std::vector<Button> buttons;
    ClippingRect* activeClipWindow = nullptr;

    // UI Helpers
    void initButtons();
    void initMenus();
    void handleButtonClick(Button& btn);
    void createDemoShape();
    
    // Features
    void undo();
    void redo();
    void saveScene();
    void loadScene();
    void performClipping();
    void performPolygonClipping();
    void toggleAnimation();
    void toggleSplitScreen();
};

#endif
