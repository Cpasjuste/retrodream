//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"

using namespace c2d;

int main(int argc, char **argv) {

    auto *renderer = new C2DRenderer(Vector2f(C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT));
    renderer->setClearColor(Color::Black);

    while (true) {

        unsigned int keys = renderer->getInput()->getKeys();
        if (keys & Input::Key::Start) {
            break;
        }

        renderer->flip();
    }

    delete (renderer);

    return 0;
}
