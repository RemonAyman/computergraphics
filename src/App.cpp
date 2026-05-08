#include "App.h"
#include "DrawingAlgorithms.h"
#include "Clipping.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

App::App() : currentMode(AppMode::NONE), isDrawing(false), isSplitScreen(false), 
             isAnimating(false), animAngle(0.0f), animScale(1.0f), scaleUp(true) {
    initButtons();
}

void App::initButtons() {
    int y = 50;
    int btnH = 25;
    int spacing = 8;

    buttons.push_back({"DDA Line", 10, y, 230, btnH, AppMode::DRAW_DDA}); y += btnH + spacing;
    buttons.push_back({"Bresenham Line", 10, y, 230, btnH, AppMode::DRAW_BRESENHAM}); y += btnH + spacing;
    buttons.push_back({"Midpoint Circle", 10, y, 230, btnH, AppMode::DRAW_CIRCLE_MIDPOINT}); y += btnH + spacing;
    buttons.push_back({"Bresenham Circle", 10, y, 230, btnH, AppMode::DRAW_CIRCLE_BRESENHAM}); y += 15 + btnH + spacing;

    buttons.push_back({"Animate (R)", 10, y, 230, btnH, AppMode::TRANSFORM_ROTATE}); y += btnH + spacing;
    buttons.push_back({"Split Compare", 10, y, 230, btnH, AppMode::NONE}); y += 15 + btnH + spacing;

    buttons.push_back({"Bezier Curve", 10, y, 230, btnH, AppMode::CURVE_BEZIER}); y += btnH + spacing;
    buttons.push_back({"Clip Window", 10, y, 230, btnH, AppMode::CLIPPING_LINE}); y += 15 + btnH + spacing;

    buttons.push_back({"Undo (Z)", 10, y, 110, btnH, AppMode::NONE});
    buttons.push_back({"Redo (Y)", 130, y, 110, btnH, AppMode::NONE}); y += btnH + spacing;
    buttons.push_back({"Save Scene", 10, y, 230, btnH, AppMode::NONE});
}

void App::initMenus() {
    int drawMenu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("DDA Line", 1);
    glutAddMenuEntry("Bresenham Line", 2);
    glutAddMenuEntry("Midpoint Circle", 3);
    
    int transMenu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("Toggle Animation", 4);
    glutAddMenuEntry("Split Screen", 5);

    glutCreateMenu(menuCallback);
    glutAddSubMenu("Drawing", drawMenu);
    glutAddSubMenu("Transformations", transMenu);
    glutAddMenuEntry("Clear All", 6);
    glutAddMenuEntry("Exit", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void App::init(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Implementation of Fundamental Computer Graphics Algorithms");

    glClearColor(APP_COLOR_BG.r, APP_COLOR_BG.g, APP_COLOR_BG.b, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    initMenus();
}

void App::run() {
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMouseFunc(mouseCallback);
    glutPassiveMotionFunc(passiveMouseCallback);
    glutTimerFunc(16, timerCallback, 0);
    glutMainLoop();
}

void App::timerCallback(int value) {
    App& app = getInstance();
    if (app.isAnimating) {
        for (auto& shape : app.shapes) {
            shape->rotate(2.0f, {WINDOW_WIDTH/2, WINDOW_HEIGHT/2});
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0);
}

void App::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    renderGrid();

    if (isSplitScreen) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2i((WINDOW_WIDTH + SIDEBAR_WIDTH) / 2, 0);
        glVertex2i((WINDOW_WIDTH + SIDEBAR_WIDTH) / 2, WINDOW_HEIGHT);
        glEnd();
    }

    for (auto& shape : shapes) shape->draw();

    if (isDrawing && !inputPoints.empty()) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2i(inputPoints[0].x, inputPoints[0].y);
        glVertex2i(mousePos.x, mousePos.y);
        glEnd();
    }

    renderUI();
    glutSwapBuffers();
}

void App::renderGrid() {
    glColor3f(APP_COLOR_GRID.r, APP_COLOR_GRID.g, APP_COLOR_GRID.b);
    glBegin(GL_LINES);
    for (int i = SIDEBAR_WIDTH; i < WINDOW_WIDTH; i += 50) {
        glVertex2i(i, 0); glVertex2i(i, WINDOW_HEIGHT);
    }
    for (int i = 0; i < WINDOW_HEIGHT; i += 50) {
        glVertex2i(SIDEBAR_WIDTH, i); glVertex2i(WINDOW_WIDTH, i);
    }
    glEnd();
}

void App::renderUI() { renderSidebar(); renderStatusBar(); }

void App::renderSidebar() {
    glColor3f(APP_COLOR_SIDEBAR.r, APP_COLOR_SIDEBAR.g, APP_COLOR_SIDEBAR.b);
    glBegin(GL_QUADS);
    glVertex2i(0, 0); glVertex2i(SIDEBAR_WIDTH, 0);
    glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT); glVertex2i(0, WINDOW_HEIGHT);
    glEnd();

    glColor3f(0.0f, 0.7f, 1.0f);
    drawText(10, 30, "GRAPHICS ENGINE v1.0", GLUT_BITMAP_HELVETICA_18);

    for (auto& btn : buttons) {
        if (btn.isHovered) glColor3f(0.35f, 0.35f, 0.35f);
        else if (currentMode == btn.mode && btn.mode != AppMode::NONE) glColor3f(APP_COLOR_ACCENT.r, APP_COLOR_ACCENT.g, APP_COLOR_ACCENT.b);
        else glColor3f(0.25f, 0.25f, 0.25f);

        glBegin(GL_QUADS);
        glVertex2i(btn.x, btn.y); glVertex2i(btn.x + btn.w, btn.y);
        glVertex2i(btn.x + btn.w, btn.y + btn.h); glVertex2i(btn.x, btn.y + btn.h);
        glEnd();

        glColor3f(1, 1, 1);
        drawText(btn.x + 10, btn.y + 18, btn.label, GLUT_BITMAP_HELVETICA_12);
    }
}

void App::renderStatusBar() {
    glColor3f(0.05f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT - 35); glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT - 35);
    glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT); glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT);
    glEnd();

    PerformanceStats stats = DrawingAlgorithms::getLastStats();
    std::stringstream ss;
    ss << "ALGO: " << stats.algoName << " | PIXELS: " << stats.pixelCount << " | TIME: " << stats.executionTime << "us | POS: (" << mousePos.x << "," << mousePos.y << ")";
    glColor3f(0.0f, 0.9f, 0.5f);
    drawText(SIDEBAR_WIDTH + 20, WINDOW_HEIGHT - 12, ss.str());
}

void App::handleButtonClick(Button& btn) {
    if (btn.label == "Undo (Z)") undo();
    else if (btn.label == "Redo (Y)") redo();
    else if (btn.label == "Split Compare") toggleSplitScreen();
    else if (btn.label == "Animate (R)") toggleAnimation();
    else if (btn.label == "Save Scene") saveScene();
    else {
        currentMode = btn.mode;
        createDemoShape();
    }
}

void App::createDemoShape() {
    Point center = { (WINDOW_WIDTH + SIDEBAR_WIDTH) / 2, WINDOW_HEIGHT / 2 };
    if (currentMode == AppMode::DRAW_DDA || currentMode == AppMode::DRAW_BRESENHAM) {
        shapes.push_back(std::make_unique<LineShape>(Point(center.x - 100, center.y), Point(center.x + 100, center.y), APP_COLOR_WHITE, (currentMode == AppMode::DRAW_DDA ? "DDA" : "Bresenham")));
    } else if (currentMode == AppMode::DRAW_CIRCLE_MIDPOINT || currentMode == AppMode::DRAW_CIRCLE_BRESENHAM) {
        shapes.push_back(std::make_unique<CircleShape>(center, 80, APP_COLOR_WHITE, (currentMode == AppMode::DRAW_CIRCLE_MIDPOINT ? "Midpoint" : "Bresenham")));
    }
}

void App::menuCallback(int id) {
    App& app = getInstance();
    switch(id) {
        case 1: app.currentMode = AppMode::DRAW_DDA; break;
        case 2: app.currentMode = AppMode::DRAW_BRESENHAM; break;
        case 4: app.toggleAnimation(); break;
        case 5: app.toggleSplitScreen(); break;
        case 6: app.shapes.clear(); break;
        case 7: exit(0);
    }
    glutPostRedisplay();
}

void App::drawText(int x, int y, std::string text, void* font) {
    glRasterPos2i(x, y);
    for (char c : text) glutBitmapCharacter(font, c);
}

void App::displayCallback() { getInstance().render(); }
void App::reshapeCallback(int w, int h) { 
    glViewport(0, 0, w, h); 
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
}
void App::mouseCallback(int button, int state, int x, int y) {
    App& app = getInstance();
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (x < SIDEBAR_WIDTH) {
            for (auto& btn : app.buttons) if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) app.handleButtonClick(btn);
        } else {
            if (app.currentMode != AppMode::NONE) {
                if (!app.isDrawing) { app.inputPoints.clear(); app.inputPoints.push_back({x, y}); app.isDrawing = true; }
                else {
                    if (app.currentMode == AppMode::DRAW_DDA || app.currentMode == AppMode::DRAW_BRESENHAM)
                        app.shapes.push_back(std::make_unique<LineShape>(app.inputPoints[0], Point(x, y), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_DDA ? "DDA" : "Bresenham")));
                    else if (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT || app.currentMode == AppMode::DRAW_CIRCLE_BRESENHAM)
                        app.shapes.push_back(std::make_unique<CircleShape>(app.inputPoints[0], (int)std::sqrt(std::pow(x-app.inputPoints[0].x,2)+std::pow(y-app.inputPoints[0].y,2)), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT ? "Midpoint" : "Bresenham")));
                    app.isDrawing = false;
                }
            }
        }
    }
    glutPostRedisplay();
}
void App::passiveMouseCallback(int x, int y) {
    App& app = getInstance(); app.mousePos = {x, y};
    for (auto& btn : app.buttons) btn.isHovered = (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
    glutPostRedisplay();
}
void App::undo() { if (!shapes.empty()) { redoStack.push_back(std::move(shapes.back())); shapes.pop_back(); } }
void App::redo() { if (!redoStack.empty()) { shapes.push_back(std::move(redoStack.back())); redoStack.pop_back(); } }
void App::keyboardCallback(unsigned char key, int x, int y) {
    App& app = getInstance();
    if (key == 'z' || key == 'Z') app.undo();
    if (key == 'y' || key == 'Y') app.redo();
    if (key == 'r' || key == 'R') app.toggleAnimation();
    if (key == 27) exit(0);
    glutPostRedisplay();
}
void App::saveScene() {
    std::ofstream file("scene.txt");
    for (auto& s : shapes) file << (int)s->getType() << " " << s->getColor().r << " " << s->getColor().g << " " << s->getColor().b << "\n";
    file.close();
}
