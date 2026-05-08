#include "App.h"

int main(int argc, char** argv) {
    App::getInstance().init(argc, argv);
    App::getInstance().run();
    return 0;
}
