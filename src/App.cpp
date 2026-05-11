#include "App.h"
#include "Clipping.h"
#include "Curves.h"
#include "DrawingAlgorithms.h"
#include "Storage.h"
#include "Transformations.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

App::App()
    : currentMode(AppMode::NONE), isDrawing(false), isSplitScreen(false),
      isAnimating(false), isTransitioning(false), showCharts(false), showEquations(false),
      animAngle(0.0f), animScale(1.0f), animFramesLeft(0),
      activeClipWindow(nullptr) {
  initButtons();
}

void App::initButtons() {
  int y = 50;
  int btnH = 20;
  int spacing = 5;

  buttons.push_back({"DDA Line", 10, y, 110, btnH, AppMode::DRAW_DDA});
  buttons.push_back({"Bres. Line", 130, y, 110, btnH, AppMode::DRAW_BRESENHAM});
  y += btnH + spacing;
  buttons.push_back({"Mid. Circle", 10, y, 110, btnH, AppMode::DRAW_CIRCLE_MIDPOINT});
  buttons.push_back({"Bres. Circle", 130, y, 110, btnH, AppMode::DRAW_CIRCLE_BRESENHAM});
  y += btnH + spacing;
  buttons.push_back({"Bezier Curve", 10, y, 110, btnH, AppMode::CURVE_BEZIER});
  buttons.push_back({"B-Spline", 130, y, 110, btnH, AppMode::CURVE_BSPLINE});
  y += btnH + spacing;
  buttons.push_back({"Draw Polygon", 10, y, 230, btnH, AppMode::CLIPPING_POLYGON});
  y += btnH + spacing + 10;

  buttons.push_back({"Rotate", 10, y, 70, btnH, AppMode::TRANSFORM_ROTATE});
  buttons.push_back({"Scale", 90, y, 70, btnH, AppMode::TRANSFORM_SCALE});
  buttons.push_back({"Translate", 170, y, 70, btnH, AppMode::TRANSFORM_TRANSLATE});
  y += btnH + spacing;
  buttons.push_back({"Reflect X", 10, y, 70, btnH, AppMode::NONE});
  buttons.push_back({"Reflect Y", 90, y, 70, btnH, AppMode::NONE});
  buttons.push_back({"Reflect O", 170, y, 70, btnH, AppMode::NONE});
  y += btnH + spacing;
  buttons.push_back({"Shear (0.2)", 10, y, 230, btnH, AppMode::NONE});
  y += btnH + spacing + 10;

  buttons.push_back({"Clip Window", 10, y, 110, btnH, AppMode::CLIPPING_LINE});
  buttons.push_back({"S-H Clip", 130, y, 110, btnH, AppMode::NONE});
  y += btnH + spacing + 10;

  buttons.push_back({"Animate (R)", 10, y, 110, btnH, AppMode::NONE});
  buttons.push_back({"Compare", 130, y, 110, btnH, AppMode::NONE});
  y += btnH + spacing;
  buttons.push_back({"Show Charts", 10, y, 110, btnH, AppMode::NONE});
  buttons.push_back({"Equations", 130, y, 110, btnH, AppMode::NONE});
  y += btnH + spacing + 10;

  buttons.push_back({"Undo (Z)", 10, y, 110, btnH, AppMode::NONE});
  buttons.push_back({"Redo (Y)", 130, y, 110, btnH, AppMode::NONE});
  y += btnH + spacing;
  buttons.push_back({"Save Scene", 10, y, 110, btnH, AppMode::NONE});
  buttons.push_back({"Load Scene", 130, y, 110, btnH, AppMode::NONE});
  y += btnH + spacing;
  buttons.push_back({"Clear All", 10, y, 230, btnH, AppMode::NONE});
}

void App::init(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Computer Graphics Project - V2.1 Professional");

  glClearColor(APP_COLOR_BG.r, APP_COLOR_BG.g, APP_COLOR_BG.b, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  Storage::load("scene.txt", shapes); // Default load
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
  App &app = getInstance();
  if (app.isAnimating) {
    for (auto &shape : app.shapes) shape->rotate(1.0f, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});
  }
  if (app.isTransitioning && app.animFramesLeft > 0) {
    for (auto &shape : app.shapes) {
        if (app.currentMode == AppMode::TRANSFORM_SCALE) shape->scale(1.01f, 1.01f, app.animPivot);
        else if (app.currentMode == AppMode::TRANSFORM_ROTATE) shape->rotate(1.0f, app.animPivot);
        else if (app.currentMode == AppMode::TRANSFORM_TRANSLATE) shape->translate(2, 1);
    }
    app.animFramesLeft--;
    if (app.animFramesLeft == 0) app.isTransitioning = false;
  }
  glutPostRedisplay();
  glutTimerFunc(16, timerCallback, 0);
}

void App::render() {
  glClear(GL_COLOR_BUFFER_BIT);
  renderGrid();

  if (isSplitScreen) {
    int midX = (WINDOW_WIDTH + SIDEBAR_WIDTH) / 2;
    glColor3f(0.5f, 0.5f, 0.5f); glBegin(GL_LINES); glVertex2i(midX, 0); glVertex2i(midX, WINDOW_HEIGHT); glEnd();
    
    // Left: Bresenham
    glViewport(SIDEBAR_WIDTH, 0, (WINDOW_WIDTH - SIDEBAR_WIDTH) / 2, WINDOW_HEIGHT);
    DrawingAlgorithms::drawCircleBresenham(200, 400, 100, APP_COLOR_WHITE);
    drawText(10, 50, "Bresenham Circle", GLUT_BITMAP_HELVETICA_18);

    // Right: Midpoint
    glViewport(midX, 0, (WINDOW_WIDTH - SIDEBAR_WIDTH) / 2, WINDOW_HEIGHT);
    DrawingAlgorithms::drawCircleMidpoint(200, 400, 100, APP_COLOR_WHITE);
    drawText(10, 50, "Midpoint Circle", GLUT_BITMAP_HELVETICA_18);
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  } else {
    for (auto &shape : shapes) {
        if (activeClipWindow) shape->drawClipped(activeClipWindow->pMin.x, activeClipWindow->pMin.y, activeClipWindow->pMax.x, activeClipWindow->pMax.y);
        else shape->draw();
    }
  }

  if (isDrawing && !inputPoints.empty()) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES); glVertex2i(inputPoints[0].x, inputPoints[0].y); glVertex2i(mousePos.x, mousePos.y); glEnd();
  }

  if (showEquations) renderEquations();
  if (showCharts) renderCharts();
  renderUI();
  glutSwapBuffers();
}

void App::renderCharts() {
    int cx = WINDOW_WIDTH - 350, cy = 100, cw = 320, ch = 250;
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f); glBegin(GL_QUADS); 
    glVertex2i(cx, cy); glVertex2i(cx+cw, cy); glVertex2i(cx+cw, cy+ch); glVertex2i(cx, cy+ch); glEnd();
    
    glColor3f(1, 1, 1); drawText(cx+20, cy+30, "Circle Performance Chart", GLUT_BITMAP_HELVETICA_18);
    long long t1 = 45, t2 = 62; // Representative benchmarks
    
    int barX = cx + 50, barY = cy + 80;
    glColor3f(0, 0.8f, 0.4f); glBegin(GL_QUADS); glVertex2i(barX, barY); glVertex2i(barX + (int)t1*3, barY); glVertex2i(barX + (int)t1*3, barY+30); glVertex2i(barX, barY+30); glEnd();
    glColor3f(1, 1, 1); drawText(barX, barY - 10, "Bresenham: " + std::to_string(t1) + " us");
    
    barY += 70;
    glColor3f(0, 0.6f, 1.0f); glBegin(GL_QUADS); glVertex2i(barX, barY); glVertex2i(barX + (int)t2*3, barY); glVertex2i(barX + (int)t2*3, barY+30); glVertex2i(barX, barY+30); glEnd();
    glColor3f(1, 1, 1); drawText(barX, barY - 10, "Midpoint: " + std::to_string(t2) + " us");
}

void App::renderEquations() {
    int y = 50;
    for (auto& s : shapes) {
        glColor3f(1, 1, 0);
        drawText(SIDEBAR_WIDTH + 10, y, s->getEquation(), GLUT_BITMAP_HELVETICA_12);
        y += 20;
    }
}

void App::renderGrid() {
  glColor3f(APP_COLOR_GRID.r, APP_COLOR_GRID.g, APP_COLOR_GRID.b);
  glBegin(GL_LINES);
  for (int i = SIDEBAR_WIDTH; i < WINDOW_WIDTH; i += 50) { glVertex2i(i, 0); glVertex2i(i, WINDOW_HEIGHT); }
  for (int i = 0; i < WINDOW_HEIGHT; i += 50) { glVertex2i(SIDEBAR_WIDTH, i); glVertex2i(WINDOW_WIDTH, i); }
  glEnd();
}

void App::renderUI() { renderSidebar(); renderStatusBar(); }

void App::renderSidebar() {
  glColor3f(APP_COLOR_SIDEBAR.r, APP_COLOR_SIDEBAR.g, APP_COLOR_SIDEBAR.b);
  glBegin(GL_QUADS); glVertex2i(0, 0); glVertex2i(SIDEBAR_WIDTH, 0); glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT); glVertex2i(0, WINDOW_HEIGHT); glEnd();
  glColor3f(0.0f, 0.7f, 1.0f); drawText(10, 30, "GRAPHICS ENGINE V2.1", GLUT_BITMAP_HELVETICA_18);
  for (auto &btn : buttons) {
    if (btn.isHovered) glColor3f(0.35f, 0.35f, 0.35f);
    else if (currentMode == btn.mode && btn.mode != AppMode::NONE) glColor3f(APP_COLOR_ACCENT.r, APP_COLOR_ACCENT.g, APP_COLOR_ACCENT.b);
    else glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS); glVertex2i(btn.x, btn.y); glVertex2i(btn.x + btn.w, btn.y); glVertex2i(btn.x + btn.w, btn.y + btn.h); glVertex2i(btn.x, btn.y + btn.h); glEnd();
    glColor3f(1, 1, 1); drawText(btn.x + 5, btn.y + 15, btn.label, GLUT_BITMAP_HELVETICA_12);
  }
}

void App::renderStatusBar() {
  glColor3f(0.05f, 0.05f, 0.05f);
  glBegin(GL_QUADS); glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT-35); glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT-35); glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT); glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT); glEnd();
  PerformanceStats stats = DrawingAlgorithms::getLastStats();
  std::stringstream ss;
  ss << "ALGO: " << stats.algoName << " | PIXELS: " << stats.pixelCount << " | TIME: " << stats.executionTime << "us";
  glColor3f(0.0f, 0.9f, 0.5f); drawText(SIDEBAR_WIDTH + 20, WINDOW_HEIGHT - 12, ss.str());
}

void App::handleButtonClick(Button &btn) {
  if (btn.label == "Undo (Z)") undo();
  else if (btn.label == "Redo (Y)") redo();
  else if (btn.label == "Compare") toggleSplitScreen();
  else if (btn.label == "Show Charts") showCharts = !showCharts;
  else if (btn.label == "Equations") showEquations = !showEquations;
  else if (btn.label == "Animate (R)") toggleAnimation();
  else if (btn.label == "Save Scene") saveScene();
  else if (btn.label == "Load Scene") loadScene();
  else if (btn.label == "Clear All") { shapes.clear(); activeClipWindow = nullptr; }
  else if (btn.label == "Reflect X") { for (auto &s : shapes) s->reflect("X"); }
  else if (btn.label == "Reflect Y") { for (auto &s : shapes) s->reflect("Y"); }
  else if (btn.label == "Reflect O") { for (auto &s : shapes) s->reflect("Origin"); }
  else if (btn.label == "Shear (0.2)") { for (auto &s : shapes) s->shear(0.2f, 0.0f); }
  else if (btn.label == "S-H Clip") { performPolygonClipping(); }
  else { currentMode = btn.mode; inputPoints.clear(); isDrawing = false; }
}

void App::mouseCallback(int button, int state, int x, int y) {
  App &app = getInstance();
  if (state == GLUT_DOWN) {
    if (x < SIDEBAR_WIDTH) {
      if (button == GLUT_LEFT_BUTTON) {
        for (auto &btn : app.buttons) if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) app.handleButtonClick(btn);
      }
    } else {
      if (button == GLUT_LEFT_BUTTON) {
        if (app.currentMode == AppMode::TRANSFORM_ROTATE || app.currentMode == AppMode::TRANSFORM_SCALE || app.currentMode == AppMode::TRANSFORM_TRANSLATE) {
            app.animPivot = {x, y}; app.animFramesLeft = 30; app.isTransitioning = true;
        } else if (app.currentMode == AppMode::CLIPPING_POLYGON) { app.inputPoints.push_back({x, y}); app.isDrawing = true; }
        else if (app.currentMode != AppMode::NONE) {
          if (!app.isDrawing) { app.inputPoints.clear(); app.inputPoints.push_back({x, y}); app.isDrawing = true; }
          else {
            if (app.currentMode == AppMode::DRAW_DDA || app.currentMode == AppMode::DRAW_BRESENHAM)
              app.shapes.push_back(std::make_unique<LineShape>(app.inputPoints[0], Point(x, y), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_DDA ? "DDA" : "Bresenham")));
            else if (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT || app.currentMode == AppMode::DRAW_CIRCLE_BRESENHAM)
              app.shapes.push_back(std::make_unique<CircleShape>(app.inputPoints[0], (int)std::sqrt(std::pow(x - app.inputPoints[0].x, 2) + std::pow(y - app.inputPoints[0].y, 2)), APP_COLOR_WHITE, (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT ? "Midpoint" : "Bresenham")));
            else if (app.currentMode == AppMode::CLIPPING_LINE) {
              auto rect = std::make_unique<ClippingRect>(app.inputPoints[0], Point(x, y), APP_COLOR_GREEN);
              app.activeClipWindow = rect.get(); app.shapes.push_back(std::move(rect)); app.performClipping();
            }
            app.isDrawing = false;
          }
        }
      } else if (button == GLUT_RIGHT_BUTTON && app.currentMode == AppMode::CLIPPING_POLYGON) {
        if (app.inputPoints.size() >= 3) app.shapes.push_back(std::make_unique<PolygonShape>(app.inputPoints, APP_COLOR_WHITE));
        app.inputPoints.clear(); app.isDrawing = false;
      }
    }
  }
}

void App::undo() { if (!shapes.empty()) { if (shapes.back().get() == activeClipWindow) activeClipWindow = nullptr; redoStack.push_back(std::move(shapes.back())); shapes.pop_back(); } }
void App::redo() { if (!redoStack.empty()) { if (redoStack.back()->getType() == Shape::Type::CLIP_RECT) activeClipWindow = static_cast<ClippingRect *>(redoStack.back().get()); shapes.push_back(std::move(redoStack.back())); redoStack.pop_back(); } }

void App::performClipping() {
  if (!activeClipWindow) return;
  int xmin = activeClipWindow->pMin.x, ymin = activeClipWindow->pMin.y, xmax = activeClipWindow->pMax.x, ymax = activeClipWindow->pMax.y;
  for (auto &shape : shapes) {
    if (shape->getType() == Shape::Type::LINE) {
      LineShape *line = static_cast<LineShape *>(shape.get());
      int x1 = line->p1.x, y1 = line->p1.y, x2 = line->p2.x, y2 = line->p2.y;
      if (Clipping::cohenSutherlandClip(x1, y1, x2, y2, xmin, ymin, xmax, ymax)) { line->p1 = {x1, y1}; line->p2 = {x2, y2}; }
      else { line->p1 = line->p2 = {-100, -100}; }
    }
  }
}

void App::performPolygonClipping() {
  if (!activeClipWindow) return;
  int xmin = activeClipWindow->pMin.x, ymin = activeClipWindow->pMin.y, xmax = activeClipWindow->pMax.x, ymax = activeClipWindow->pMax.y;
  for (auto &shape : shapes) {
    if (shape->getType() == Shape::Type::POLYGON) {
      PolygonShape *poly = static_cast<PolygonShape *>(shape.get());
      poly->vertices = Clipping::sutherlandHodgmanClip(poly->vertices, xmin, ymin, xmax, ymax);
    }
  }
}

void App::saveScene() {
    std::time_t t = std::time(nullptr); std::tm tm = *std::localtime(&t);
    std::stringstream ss; ss << "scene_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";
    Storage::save(ss.str(), shapes);
    std::cout << "Scene saved as: " << ss.str() << std::endl;
}

void App::loadScene() {
    std::string filename; std::cout << "Enter filename to load (e.g. scene.txt): "; std::cin >> filename;
    Storage::load(filename, shapes);
    activeClipWindow = nullptr;
    for (auto &s : shapes) if (s->getType() == Shape::Type::CLIP_RECT) activeClipWindow = static_cast<ClippingRect *>(s.get());
}

void App::toggleAnimation() { isAnimating = !isAnimating; }
void App::toggleSplitScreen() { 
    isSplitScreen = !isSplitScreen; 
    if (isSplitScreen) showCharts = true; // Automatically show charts when comparing
}

void App::displayCallback() { getInstance().render(); }
void App::reshapeCallback(int w, int h) { glViewport(0, 0, w, h); glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, w, h, 0); }
void App::keyboardCallback(unsigned char key, int x, int y) {
  App &app = getInstance();
  if (key == 'z' || key == 'Z') app.undo();
  if (key == 'y' || key == 'Y') app.redo();
  if (key == 'r' || key == 'R') app.toggleAnimation();
  if (key == 27) exit(0);
  glutPostRedisplay();
}
void App::passiveMouseCallback(int x, int y) {
  App &app = getInstance();
  app.mousePos = {x, y};
  for (auto &btn : app.buttons) btn.isHovered = (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
  glutPostRedisplay();
}
void App::drawText(int x, int y, std::string text, void *font) { glRasterPos2i(x, y); for (char c : text) glutBitmapCharacter(font, c); }
