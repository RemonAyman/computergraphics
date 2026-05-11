#include "App.h"
#include "DrawingAlgorithms.h"
#include "Clipping.h"
#include "Curves.h"
#include "Storage.h"
#include "Transformations.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

App::App() : currentMode(AppMode::NONE), isDrawing(false), isSplitScreen(false), 
             isAnimating(false), animAngle(0.0f), animScale(1.0f), scaleUp(true), activeClipWindow(nullptr) {
    initButtons();
}

void App::initButtons() {
    int y = 50;
    int btnH = 20;
    int spacing = 5;

    buttons.push_back({"DDA Line", 10, y, 110, btnH, AppMode::DRAW_DDA}); 
    buttons.push_back({"Bres. Line", 130, y, 110, btnH, AppMode::DRAW_BRESENHAM}); y += btnH + spacing;
    buttons.push_back({"Mid. Circle", 10, y, 110, btnH, AppMode::DRAW_CIRCLE_MIDPOINT}); 
    buttons.push_back({"Bres. Circle", 130, y, 110, btnH, AppMode::DRAW_CIRCLE_BRESENHAM}); y += btnH + spacing;
    
    buttons.push_back({"Bezier Curve", 10, y, 110, btnH, AppMode::CURVE_BEZIER}); 
    buttons.push_back({"B-Spline", 130, y, 110, btnH, AppMode::CURVE_BSPLINE}); y += btnH + spacing;
    
    buttons.push_back({"Draw Polygon", 10, y, 230, btnH, AppMode::CLIPPING_POLYGON}); y += btnH + spacing + 10;

    buttons.push_back({"Reflect X", 10, y, 70, btnH, AppMode::NONE});
    buttons.push_back({"Reflect Y", 90, y, 70, btnH, AppMode::NONE});
    buttons.push_back({"Reflect O", 170, y, 70, btnH, AppMode::NONE}); y += btnH + spacing;
    
    buttons.push_back({"Shear (0.2)", 10, y, 230, btnH, AppMode::NONE}); y += btnH + spacing + 10;

    buttons.push_back({"Clip Window", 10, y, 110, btnH, AppMode::CLIPPING_LINE}); 
    buttons.push_back({"S-H Clip", 130, y, 110, btnH, AppMode::NONE}); y += btnH + spacing + 10;

    buttons.push_back({"Animate (R)", 10, y, 110, btnH, AppMode::NONE});
    buttons.push_back({"Compare", 130, y, 110, btnH, AppMode::NONE}); y += btnH + spacing + 10;

    buttons.push_back({"Undo (Z)", 10, y, 110, btnH, AppMode::NONE});
    buttons.push_back({"Redo (Y)", 130, y, 110, btnH, AppMode::NONE}); y += btnH + spacing;
    buttons.push_back({"Save Scene", 10, y, 110, btnH, AppMode::NONE});
    buttons.push_back({"Load Scene", 130, y, 110, btnH, AppMode::NONE}); y += btnH + spacing;
    buttons.push_back({"Clear All", 10, y, 230, btnH, AppMode::NONE});
}

void App::init(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Computer Graphics Project - Final Implementation");

    glClearColor(APP_COLOR_BG.r, APP_COLOR_BG.g, APP_COLOR_BG.b, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    loadScene();
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
        if (currentMode == AppMode::CLIPPING_POLYGON) {
            glBegin(GL_LINE_STRIP);
            for (const auto& p : inputPoints) glVertex2i(p.x, p.y);
            glVertex2i(mousePos.x, mousePos.y);
            glEnd();
        } else {
            glBegin(GL_LINES);
            glVertex2i(inputPoints[0].x, inputPoints[0].y);
            glVertex2i(mousePos.x, mousePos.y);
            glEnd();
        }
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
    drawText(10, 30, "GRAPHICS ENGINE PRO", GLUT_BITMAP_HELVETICA_18);

    for (auto& btn : buttons) {
        if (btn.isHovered) glColor3f(0.35f, 0.35f, 0.35f);
        else if (currentMode == btn.mode && btn.mode != AppMode::NONE) glColor3f(APP_COLOR_ACCENT.r, APP_COLOR_ACCENT.g, APP_COLOR_ACCENT.b);
        else glColor3f(0.25f, 0.25f, 0.25f);

        glBegin(GL_QUADS);
        glVertex2i(btn.x, btn.y); glVertex2i(btn.x + btn.w, btn.y);
        glVertex2i(btn.x + btn.w, btn.y + btn.h); glVertex2i(btn.x, btn.y + btn.h);
        glEnd();

        glColor3f(1, 1, 1);
        drawText(btn.x + 5, btn.y + 15, btn.label, GLUT_BITMAP_HELVETICA_12);
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
    else if (btn.label == "Compare") toggleSplitScreen();
    else if (btn.label == "Animate (R)") toggleAnimation();
    else if (btn.label == "Save Scene") saveScene();
    else if (btn.label == "Load Scene") loadScene();
    else if (btn.label == "Clear All") { shapes.clear(); activeClipWindow = nullptr; }
    else if (btn.label == "Reflect X") { for (auto& s : shapes) s->reflect("X"); }
    else if (btn.label == "Reflect Y") { for (auto& s : shapes) s->reflect("Y"); }
    else if (btn.label == "Reflect O") { for (auto& s : shapes) s->reflect("Origin"); }
    else if (btn.label == "Shear (0.2)") { for (auto& s : shapes) s->shear(0.2f, 0.0f); }
    else if (btn.label == "S-H Clip") { performPolygonClipping(); }
    else {
        currentMode = btn.mode;
        inputPoints.clear();
        isDrawing = false;
    }
}

void App::mouseCallback(int button, int state, int x, int y) {
    App& app = getInstance();
    if (state == GLUT_DOWN) {
        if (x < SIDEBAR_WIDTH) {
            if (button == GLUT_LEFT_BUTTON) {
                for (auto& btn : app.buttons) if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) app.handleButtonClick(btn);
            }
        } else {
            if (button == GLUT_LEFT_BUTTON) {
                if (app.currentMode == AppMode::CLIPPING_POLYGON) {
                    app.inputPoints.push_back({x, y});
                    app.isDrawing = true;
                } else if (app.currentMode == AppMode::CURVE_BEZIER || app.currentMode == AppMode::CURVE_BSPLINE) {
                    app.inputPoints.push_back({x, y});
                    if (app.inputPoints.size() == 4) {
                        if (app.currentMode == AppMode::CURVE_BEZIER) app.shapes.push_back(std::make_unique<BezierShape>(app.inputPoints, APP_COLOR_WHITE, "Bezier"));
                        else app.shapes.push_back(std::make_unique<BezierShape>(app.inputPoints, APP_COLOR_RED, "BSpline"));
                        app.inputPoints.clear(); app.isDrawing = false;
                    } else { app.isDrawing = true; }
                } else if (app.currentMode != AppMode::NONE) {
                    if (!app.isDrawing) { app.inputPoints.clear(); app.inputPoints.push_back({x, y}); app.isDrawing = true; }
                    else {
                        if (app.currentMode == AppMode::DRAW_DDA || app.currentMode == AppMode::DRAW_BRESENHAM)
                            app.shapes.push_back(std::make_unique<LineShape>(app.inputPoints[0], Point(x, y), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_DDA ? "DDA" : "Bresenham")));
                        else if (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT || app.currentMode == AppMode::DRAW_CIRCLE_BRESENHAM)
                            app.shapes.push_back(std::make_unique<CircleShape>(app.inputPoints[0], (int)std::sqrt(std::pow(x-app.inputPoints[0].x,2)+std::pow(y-app.inputPoints[0].y,2)), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT ? "Midpoint" : "Bresenham")));
                        else if (app.currentMode == AppMode::CLIPPING_LINE) {
                            auto rect = std::make_unique<ClippingRect>(app.inputPoints[0], Point(x, y), APP_COLOR_GREEN);
                            app.activeClipWindow = rect.get();
                            app.shapes.push_back(std::move(rect));
                            app.performClipping();
                        }
                        app.isDrawing = false;
                    }
                }
            } else if (button == GLUT_RIGHT_BUTTON && app.currentMode == AppMode::CLIPPING_POLYGON) {
                if (app.inputPoints.size() >= 3) {
                    app.shapes.push_back(std::make_unique<PolygonShape>(app.inputPoints, APP_COLOR_WHITE));
                }
                app.inputPoints.clear(); app.isDrawing = false;
            }
        }
    }
    glutPostRedisplay();
}

void App::undo() { if (!shapes.empty()) { if (shapes.back().get() == activeClipWindow) activeClipWindow = nullptr; redoStack.push_back(std::move(shapes.back())); shapes.pop_back(); } }
void App::redo() { if (!redoStack.empty()) { if (redoStack.back()->getType() == Shape::Type::CLIP_RECT) activeClipWindow = static_cast<ClippingRect*>(redoStack.back().get()); shapes.push_back(std::move(redoStack.back())); redoStack.pop_back(); } }

void App::performClipping() {
    if (!activeClipWindow) return;
    int xmin = activeClipWindow->pMin.x, ymin = activeClipWindow->pMin.y;
    int xmax = activeClipWindow->pMax.x, ymax = activeClipWindow->pMax.y;
    for (auto& shape : shapes) {
        if (shape->getType() == Shape::Type::LINE) {
            LineShape* line = static_cast<LineShape*>(shape.get());
            int x1 = line->p1.x, y1 = line->p1.y, x2 = line->p2.x, y2 = line->p2.y;
            if (Clipping::cohenSutherlandClip(x1, y1, x2, y2, xmin, ymin, xmax, ymax)) { line->p1 = {x1, y1}; line->p2 = {x2, y2}; }
            else { line->p1 = line->p2 = {-100, -100}; }
        }
    }
}

void App::performPolygonClipping() {
    if (!activeClipWindow) return;
    int xmin = activeClipWindow->pMin.x, ymin = activeClipWindow->pMin.y;
    int xmax = activeClipWindow->pMax.x, ymax = activeClipWindow->pMax.y;
    for (auto& shape : shapes) {
        if (shape->getType() == Shape::Type::POLYGON) {
            PolygonShape* poly = static_cast<PolygonShape*>(shape.get());
            poly->vertices = Clipping::sutherlandHodgmanClip(poly->vertices, xmin, ymin, xmax, ymax);
        }
    }
}

void App::saveScene() { Storage::save("scene.txt", shapes); }
void App::loadScene() { Storage::load("scene.txt", shapes); activeClipWindow = nullptr; for (auto& s : shapes) if (s->getType() == Shape::Type::CLIP_RECT) activeClipWindow = static_cast<ClippingRect*>(s.get()); }
void App::toggleAnimation() { isAnimating = !isAnimating; }
void App::toggleSplitScreen() { isSplitScreen = !isSplitScreen; }

void App::displayCallback() { getInstance().render(); }
void App::reshapeCallback(int w, int h) { glViewport(0, 0, w, h); glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, w, h, 0); }
void App::keyboardCallback(unsigned char key, int x, int y) {
    App& app = getInstance();
    if (key == 'z' || key == 'Z') app.undo();
    if (key == 'y' || key == 'Y') app.redo();
    if (key == 'r' || key == 'R') app.toggleAnimation();
    if (key == 27) exit(0);
    glutPostRedisplay();
}
void App::passiveMouseCallback(int x, int y) {
    App& app = getInstance(); app.mousePos = {x, y};
    for (auto& btn : app.buttons) btn.isHovered = (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
    glutPostRedisplay();
}
void App::drawText(int x, int y, std::string text, void* font) { glRasterPos2i(x, y); for (char c : text) glutBitmapCharacter(font, c); }
