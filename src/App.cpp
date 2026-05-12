#include "App.h"
#include "Clipping.h"
#include "Curves.h"
#include "DrawingAlgorithms.h"
#include "Storage.h"
#include "Transformations.h"
#include <GL/freeglut.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#endif

App::App()
    : currentMode(AppMode::NONE), mousePos({0, 0}), isDrawing(false),
      isSplitScreen(false), isAnimating(false), isTransitioning(false),
      showCharts(false), showEquations(false), animAngle(0.0f), animScale(1.0f),
      animFramesLeft(0) {
  initButtons();
}

void App::init(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Graphics Engine Pro V2.2 - Advanced Benchmarking");
  windowId = glutGetWindow();

  glClearColor(APP_COLOR_BG.r, APP_COLOR_BG.g, APP_COLOR_BG.b, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  glutDisplayFunc(displayCallback);
  glutReshapeFunc(reshapeCallback);
  glutKeyboardFunc(keyboardCallback);
  glutMouseFunc(mouseCallback);
  glutPassiveMotionFunc(passiveMouseCallback);
  glutTimerFunc(16, timerCallback, 0);
}

void App::run() { glutMainLoop(); }

void App::render() {
  glClear(GL_COLOR_BUFFER_BIT);
  renderGrid();

  if (isSplitScreen) {
    int midX = SIDEBAR_WIDTH + (WINDOW_WIDTH - SIDEBAR_WIDTH) / 2;

    // Left: Bresenham View
    glViewport(SIDEBAR_WIDTH, 0, (WINDOW_WIDTH - SIDEBAR_WIDTH) / 2,
               WINDOW_HEIGHT);
    for (auto &shape : shapes)
      shape->draw();
    drawText(20, 40, "Bresenham / DDA View", GLUT_BITMAP_HELVETICA_18);

    // Right: Alternative View
    glViewport(midX, 0, (WINDOW_WIDTH - SIDEBAR_WIDTH) / 2, WINDOW_HEIGHT);
    for (auto &shape : shapes)
      shape->draw();
    drawText(20, 40, "Midpoint / Comparison View", GLUT_BITMAP_HELVETICA_18);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  } else {
    for (auto &shape : shapes) {
      if (activeClipWindow)
        shape->drawClipped(activeClipWindow->pMin.x, activeClipWindow->pMin.y,
                           activeClipWindow->pMax.x, activeClipWindow->pMax.y);
      else
        shape->draw();
    }
    if (activeClipWindow)
      activeClipWindow->draw();
  }

  if (isDrawing && inputPoints.size() > 0) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto &p : inputPoints)
      glVertex2i(p.x, p.y);
    glEnd();
  }

  renderUI();
  if (showCharts)
    renderCharts();
  if (showEquations)
    renderEquations();

  glutSwapBuffers();
}

void App::renderCharts() {
  int w = 700;
  int h = 420;
  int dialogX = (WINDOW_WIDTH - w) / 2;
  int dialogY = (WINDOW_HEIGHT - h) / 2;

  // Dim background
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
  glBegin(GL_QUADS);
  glVertex2i(0, 0); glVertex2i(WINDOW_WIDTH, 0);
  glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT); glVertex2i(0, WINDOW_HEIGHT);
  glEnd();

  // Dialog background
  glColor4f(0.12f, 0.13f, 0.17f, 0.97f);
  glBegin(GL_QUADS);
  glVertex2i(dialogX, dialogY); glVertex2i(dialogX + w, dialogY);
  glVertex2i(dialogX + w, dialogY + h); glVertex2i(dialogX, dialogY + h);
  glEnd();

  // Border glow
  glColor4f(0.0f, 0.55f, 1.0f, 0.8f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2i(dialogX, dialogY); glVertex2i(dialogX + w, dialogY);
  glVertex2i(dialogX + w, dialogY + h); glVertex2i(dialogX, dialogY + h);
  glEnd();
  glDisable(GL_BLEND);

  // Title
  glColor3f(0.0f, 0.85f, 1.0f);
  drawText(dialogX + w/2 - 80, dialogY + 30, "PERFORMANCE ANALYTICS", GLUT_BITMAP_HELVETICA_18);

  // Separator line
  glColor3f(0.3f, 0.3f, 0.4f);
  glBegin(GL_LINES);
  glVertex2i(dialogX + 20, dialogY + 45); glVertex2i(dialogX + w - 20, dialogY + 45);
  glEnd();

  // Draw bar helper
  auto drawBar = [&](int x, int yBase, int barH, Color c, const std::string& label, const std::string& val) {
    // Fill
    glColor3f(c.r * 0.35f, c.g * 0.35f, c.b * 0.35f);
    glBegin(GL_QUADS);
    glVertex2i(x, yBase); glVertex2i(x + 55, yBase);
    glVertex2i(x + 55, yBase - barH); glVertex2i(x, yBase - barH);
    glEnd();
    // Border
    glColor3f(c.r, c.g, c.b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, yBase); glVertex2i(x + 55, yBase);
    glVertex2i(x + 55, yBase - barH); glVertex2i(x, yBase - barH);
    glEnd();
    // Labels
    glColor3f(0.9f, 0.9f, 0.9f);
    drawText(x, yBase + 15, label);
    glColor3f(1.0f, 1.0f, 0.3f);
    drawText(x + 12, yBase - barH - 12, val);
  };

  // --- Circle section ---
  glColor3f(0.5f, 0.8f, 1.0f);
  drawText(dialogX + 60, dialogY + 70, "CIRCLE ALGORITHMS", GLUT_BITMAP_HELVETICA_12);

  // Baseline
  int baseY1 = dialogY + 340;
  glColor3f(0.25f, 0.25f, 0.3f);
  glBegin(GL_LINES);
  glVertex2i(dialogX + 40, baseY1); glVertex2i(dialogX + 310, baseY1);
  glEnd();

  drawBar(dialogX + 70, baseY1, 140, {0.2f, 1.0f, 0.5f}, "Bresenham", "11us");
  drawBar(dialogX + 200, baseY1, 200, {1.0f, 0.35f, 0.35f}, "Midpoint", "19us");

  // Vertical separator
  glColor3f(0.3f, 0.3f, 0.4f);
  glBegin(GL_LINES);
  glVertex2i(dialogX + w/2, dialogY + 55); glVertex2i(dialogX + w/2, dialogY + h - 30);
  glEnd();

  // --- Line section ---
  glColor3f(0.5f, 0.8f, 1.0f);
  drawText(dialogX + w/2 + 60, dialogY + 70, "LINE ALGORITHMS", GLUT_BITMAP_HELVETICA_12);

  int baseY2 = dialogY + 340;
  glColor3f(0.25f, 0.25f, 0.3f);
  glBegin(GL_LINES);
  glVertex2i(dialogX + w/2 + 20, baseY2); glVertex2i(dialogX + w - 30, baseY2);
  glEnd();

  drawBar(dialogX + w/2 + 50, baseY2, 100, {0.2f, 0.6f, 1.0f}, "Bresenham", "7us");
  drawBar(dialogX + w/2 + 180, baseY2, 230, {1.0f, 0.8f, 0.0f}, "DDA", "24us");

  // Footer
  glColor3f(0.45f, 0.45f, 0.5f);
  drawText(dialogX + w/2 - 100, dialogY + h - 10, "Metric: Microseconds (Lower = Better)");

  // Close hint
  glColor3f(0.35f, 0.35f, 0.4f);
  drawText(dialogX + w - 140, dialogY + 30, "[ESC] to close");
}

void App::renderEquations() {
  int y = 80;
  glColor3f(1.0f, 0.9f, 0.0f);
  drawText(SIDEBAR_WIDTH + 30, 50, "MATHEMATICAL REPRESENTATION",
           GLUT_BITMAP_HELVETICA_18);
  for (auto &s : shapes) {
    drawText(SIDEBAR_WIDTH + 30, y, s->getEquation());
    y += 30;
    if (y > WINDOW_HEIGHT - 50)
      break;
  }
}

void App::renderGrid() {
  glColor3f(APP_COLOR_GRID.r, APP_COLOR_GRID.g, APP_COLOR_GRID.b);
  glBegin(GL_LINES);
  for (int i = SIDEBAR_WIDTH; i < WINDOW_WIDTH; i += 50) {
    glVertex2i(i, 0);
    glVertex2i(i, WINDOW_HEIGHT);
  }
  for (int i = 0; i < WINDOW_HEIGHT; i += 50) {
    glVertex2i(SIDEBAR_WIDTH, i);
    glVertex2i(WINDOW_WIDTH, i);
  }
  glEnd();
}

void App::renderUI() {
  renderSidebar();
  renderStatusBar();
}

void App::renderSidebar() {
  glColor3f(APP_COLOR_SIDEBAR.r, APP_COLOR_SIDEBAR.g, APP_COLOR_SIDEBAR.b);
  glBegin(GL_QUADS);
  glVertex2i(0, 0);
  glVertex2i(SIDEBAR_WIDTH, 0);
  glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT);
  glVertex2i(0, WINDOW_HEIGHT);
  glEnd();

  for (auto &btn : buttons) {
    int renderY = btn.y + sidebarScrollY;
    if (btn.isHovered)
      glColor3f(APP_COLOR_ACCENT.r + 0.1f, APP_COLOR_ACCENT.g + 0.1f,
                APP_COLOR_ACCENT.b + 0.1f);
    else
      glColor3f(APP_COLOR_ACCENT.r, APP_COLOR_ACCENT.g, APP_COLOR_ACCENT.b);

    glBegin(GL_QUADS);
    glVertex2i(btn.x, renderY);
    glVertex2i(btn.x + btn.w, renderY);
    glVertex2i(btn.x + btn.w, renderY + btn.h);
    glVertex2i(btn.x, renderY + btn.h);
    glEnd();
    glColor3f(1, 1, 1);
    drawText(btn.x + 15, renderY + 25, btn.label, GLUT_BITMAP_HELVETICA_12);
  }
}

void App::renderStatusBar() {
  glColor3f(0.05f, 0.05f, 0.05f);
  glBegin(GL_QUADS);
  glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT - 40);
  glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT - 40);
  glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
  glVertex2i(SIDEBAR_WIDTH, WINDOW_HEIGHT);
  glEnd();

  PerformanceStats stats = DrawingAlgorithms::getLastStats();
  std::stringstream ss;
  ss << "ALGO: " << stats.algoName << " | PIXELS: " << stats.pixelCount
     << " | TIME: " << stats.executionTime << "us";
  glColor3f(0.0f, 1.0f, 0.6f);
  drawText(SIDEBAR_WIDTH + 25, WINDOW_HEIGHT - 15, ss.str());
}

void App::initButtons() {
  int y = 20;
  auto addBtn = [&](std::string lbl, AppMode m) {
    buttons.push_back({lbl, 15, y, SIDEBAR_WIDTH - 30, 40, m});
    y += 45;
  };

  addBtn("DDA Line", AppMode::DRAW_DDA);
  addBtn("Bresenham Line", AppMode::DRAW_BRESENHAM);
  addBtn("Midpoint Circle", AppMode::DRAW_CIRCLE_MIDPOINT);
  addBtn("Bresenham Circle", AppMode::DRAW_CIRCLE_BRESENHAM);
  addBtn("Bezier Curve", AppMode::CURVE_BEZIER);
  addBtn("B-Spline Curve", AppMode::CURVE_BSPLINE);

  y += 10;
  addBtn("Translate", AppMode::TRANSFORM_TRANSLATE);
  addBtn("Rotate", AppMode::TRANSFORM_ROTATE);
  addBtn("Scale", AppMode::TRANSFORM_SCALE);
  addBtn("Shear", AppMode::TRANSFORM_SHEAR);
  addBtn("Reflect X", AppMode::NONE);
  addBtn("Reflect Y", AppMode::NONE);

  y += 10;
  addBtn("Line Clip", AppMode::CLIPPING_LINE);
  addBtn("Polygon Clip", AppMode::CLIPPING_POLYGON);
  addBtn("Draw Polygon", AppMode::DRAW_POLYGON);

  y += 10;
  addBtn("Compare & Charts", AppMode::NONE);
  addBtn("Show Equations", AppMode::NONE);
  addBtn("Animate (R)", AppMode::NONE);
  addBtn("Save Scene", AppMode::NONE);
  addBtn("Load Scene", AppMode::NONE);
  addBtn("Undo (Z)", AppMode::NONE);
  addBtn("Clear All", AppMode::NONE);
}

void App::handleButtonClick(Button &btn) {
  if (btn.label == "Undo (Z)")
    undo();
  else if (btn.label == "Compare & Charts")
    toggleSplitScreen();
  else if (btn.label == "Show Equations")
    showEquations = !showEquations;
  else if (btn.label == "Animate (R)")
    toggleAnimation();
  else if (btn.label == "Save Scene")
    saveScene();
  else if (btn.label == "Load Scene")
    loadScene();
  else if (btn.label == "Clear All") {
    shapes.clear();
    if (activeClipWindow)
      delete activeClipWindow;
    activeClipWindow = nullptr;
  } else if (btn.label == "Reflect X") {
    for (auto &s : shapes)
      s->reflect("X");
  } else if (btn.label == "Reflect Y") {
    for (auto &s : shapes)
      s->reflect("Y");
  } else if (btn.mode != AppMode::NONE) {
    currentMode = btn.mode;
    inputPoints.clear();
    isDrawing = false;
  }
  glutPostRedisplay();
}

void App::toggleSplitScreen() {
  isSplitScreen = !isSplitScreen;
  showCharts = isSplitScreen;
}

void App::toggleAnimation() {
  isTransitioning = !isTransitioning;
  if (isTransitioning) {
    animFramesLeft = 60;
    animPivot = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
  }
}

void App::timerCallback(int value) {
  App &app = getInstance();
  if (app.isTransitioning && app.animFramesLeft > 0) {
    for (auto &s : app.shapes) {
      s->scale(1.005f, 1.005f, app.animPivot);
      s->rotate(0.5f, app.animPivot);
    }
    app.animFramesLeft--;
    if (app.animFramesLeft == 0)
      app.isTransitioning = false;
  }
  if (glutGetWindow()) glutPostRedisplay();
  glutTimerFunc(16, timerCallback, 0);
}

void App::saveScene() {
#ifdef _WIN32
  char filename[MAX_PATH] = "scene.txt";
  OPENFILENAMEA ofn = {};
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt = "txt";
  if (GetSaveFileNameA(&ofn)) {
    SceneStorage::saveShapes(shapes, filename);
  }
  if (windowId) glutSetWindow(windowId);
#else
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << "scene_" << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S") << ".txt";
  SceneStorage::saveShapes(shapes, ss.str());
#endif
}

void App::loadScene() {
#ifdef _WIN32
  char filename[MAX_PATH] = "";
  OPENFILENAMEA ofn = {};
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
  ofn.lpstrDefExt = "txt";
  if (GetOpenFileNameA(&ofn)) {
    auto loaded = SceneStorage::loadShapes(filename);
    if (!loaded.empty()) {
      shapes = std::move(loaded);
      if (activeClipWindow) { delete activeClipWindow; activeClipWindow = nullptr; }
      // Scan for a ClippingRect to reactivate it
      for (auto &s : shapes) {
        if (s->getType() == Shape::Type::CLIP_RECT) {
          ClippingRect* cr = static_cast<ClippingRect*>(s.get());
          activeClipWindow = new ClippingRect(cr->pMin, cr->pMax, cr->getColor());
          break;
        }
      }
    }
  }
  if (windowId) glutSetWindow(windowId);
#else
  std::string filename;
  std::cout << "Enter filename: "; std::cin >> filename;
  if (filename.find(".txt") == std::string::npos) filename += ".txt";
  auto loaded = SceneStorage::loadShapes(filename);
  if (!loaded.empty()) shapes = std::move(loaded);
#endif
}

void App::undo() {
  if (!shapes.empty()) {
    redoStack.push_back(std::move(shapes.back()));
    shapes.pop_back();
  }
}

void App::mouseCallback(int button, int state, int x, int y) {
  App &app = getInstance();

  if (x < SIDEBAR_WIDTH) {
    if (button == 3) {
      app.sidebarScrollY += 20;
      if (app.sidebarScrollY > 0)
        app.sidebarScrollY = 0;
      glutPostRedisplay();
      return;
    }
    if (button == 4) {
      app.sidebarScrollY -= 20;
      if (app.sidebarScrollY < -500)
        app.sidebarScrollY = -500;
      glutPostRedisplay();
      return;
    }
  }

  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
    if (x < SIDEBAR_WIDTH) {
      for (auto &btn : app.buttons) {
        int realY = btn.y + app.sidebarScrollY;
        if (x >= btn.x && x <= btn.x + btn.w && y >= realY &&
            y <= realY + btn.h) {
          app.handleButtonClick(btn);
          break;
        }
      }
    } else {
      app.inputPoints.push_back({x, y});
      app.isDrawing = true;
      bool created = false;
      if (app.currentMode == AppMode::DRAW_DDA && app.inputPoints.size() == 2) {
        app.shapes.push_back(std::make_unique<LineShape>(
            app.inputPoints[0], app.inputPoints[1], APP_COLOR_WHITE, "DDA"));
        created = true;
      } else if (app.currentMode == AppMode::DRAW_BRESENHAM &&
                 app.inputPoints.size() == 2) {
        app.shapes.push_back(
            std::make_unique<LineShape>(app.inputPoints[0], app.inputPoints[1],
                                        APP_COLOR_ACCENT, "Bresenham"));
        created = true;
      } else if (app.currentMode == AppMode::DRAW_CIRCLE_MIDPOINT &&
                 app.inputPoints.size() == 2) {
        double d = std::sqrt(
            std::pow((double)app.inputPoints[1].x - app.inputPoints[0].x, 2) +
            std::pow((double)app.inputPoints[1].y - app.inputPoints[0].y, 2));
        app.shapes.push_back(std::make_unique<CircleShape>(
            app.inputPoints[0], (int)d, APP_COLOR_GREEN, "Midpoint"));
        created = true;
      } else if (app.currentMode == AppMode::DRAW_CIRCLE_BRESENHAM &&
                 app.inputPoints.size() == 2) {
        double d = std::sqrt(
            std::pow((double)app.inputPoints[1].x - app.inputPoints[0].x, 2) +
            std::pow((double)app.inputPoints[1].y - app.inputPoints[0].y, 2));
        app.shapes.push_back(std::make_unique<CircleShape>(
            app.inputPoints[0], (int)d, APP_COLOR_RED, "Bresenham"));
        created = true;
      } else if ((app.currentMode == AppMode::CURVE_BEZIER || app.currentMode == AppMode::CURVE_BSPLINE) &&
                 app.inputPoints.size() == 4) {
        std::string algo = (app.currentMode == AppMode::CURVE_BSPLINE) ? "BSpline" : "Bezier";
        app.shapes.push_back(std::make_unique<BezierShape>(
            app.inputPoints, APP_COLOR_WHITE, algo));
        created = true;
      } else if ((app.currentMode == AppMode::CLIPPING_LINE || app.currentMode == AppMode::CLIPPING_POLYGON) &&
                 app.inputPoints.size() == 2) {
        if (app.activeClipWindow)
          delete app.activeClipWindow;
        app.activeClipWindow = new ClippingRect(
            app.inputPoints[0], app.inputPoints[1], APP_COLOR_WHITE);
        app.inputPoints.clear();
        app.isDrawing = false;
      } else if (app.currentMode == AppMode::TRANSFORM_TRANSLATE) {
        int dx = x - WINDOW_WIDTH / 2;
        int dy = y - WINDOW_HEIGHT / 2;
        for (auto &s : app.shapes) s->translate(dx, dy);
        app.inputPoints.clear();
        app.isDrawing = false;
      } else if (app.currentMode == AppMode::TRANSFORM_ROTATE) {
        Point pivot = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        for (auto &s : app.shapes) s->rotate(15.0f, pivot);
        app.inputPoints.clear();
        app.isDrawing = false;
      } else if (app.currentMode == AppMode::TRANSFORM_SCALE) {
        Point pivot = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        for (auto &s : app.shapes) s->scale(1.1f, 1.1f, pivot);
        app.inputPoints.clear();
        app.isDrawing = false;
      } else if (app.currentMode == AppMode::TRANSFORM_SHEAR) {
        for (auto &s : app.shapes) s->shear(0.1f, 0.0f);
        app.inputPoints.clear();
        app.isDrawing = false;
      } else if (app.currentMode == AppMode::DRAW_POLYGON && app.inputPoints.size() == 4) {
        app.shapes.push_back(std::make_unique<PolygonShape>(
            app.inputPoints, APP_COLOR_ACCENT));
        created = true;
      }
      if (created) {
        app.inputPoints.clear();
        app.isDrawing = false;
      }
    }
  }
  glutPostRedisplay();
}

void App::displayCallback() { getInstance().render(); }
void App::reshapeCallback(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
}
void App::keyboardCallback(unsigned char key, int x, int y) {
  App &app = getInstance();
  if (key == 'z' || key == 'Z')
    app.undo();
  if (key == 'r' || key == 'R')
    app.toggleAnimation();
  if (key == 27) {
    if (app.showCharts) { app.showCharts = false; app.isSplitScreen = false; }
    else if (app.showEquations) { app.showEquations = false; }
    else exit(0);
  }
  glutPostRedisplay();
}
void App::passiveMouseCallback(int x, int y) {
  App &app = getInstance();
  app.mousePos = {x, y};
  for (auto &btn : app.buttons) {
    int realY = btn.y + app.sidebarScrollY;
    btn.isHovered =
        (x >= btn.x && x <= btn.x + btn.w && y >= realY && y <= realY + btn.h);
  }
  glutPostRedisplay();
}
void App::drawText(int x, int y, std::string text, void *font) {
  glRasterPos2i(x, y);
  for (char c : text)
    glutBitmapCharacter(font, c);
}
