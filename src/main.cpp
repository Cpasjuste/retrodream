//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"

using namespace c2d;

RetroDream::RetroDream(const c2d::Vector2f &size) : C2DRenderer(size) {

    setFillColor(COL_GRAY_LIGHT);
    setOutlineColor(COL_RED);
    setOutlineThickness(2);
    setPosition(1, 1);
    setSize(size.x - 4, size.y - 4);

    getFont()->setFilter(Texture::Filter::Point);

    FloatRect filerRect = {16, 64, (size.x / 2) - 32, size.y - (64 + 16)};
    filer = new Filer(this, filerRect, "/");
    filer->setFillColor(COL_GRAY);
    filer->setOutlineColor(COL_BLUE_LIGHT);
    filer->setOutlineThickness(2);
    add(filer);
}

RetroDream::~RetroDream() {

}

bool RetroDream::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        filer->up();
    } else if (keys & Input::Key::Down) {
        filer->down();
    } else if (keys & Input::Key::Right) {
        filer->setSelection(filer->getIndex() + filer->getMaxLines());
    } else if (keys & Input::Key::Left) {
        filer->setSelection(filer->getIndex() - filer->getMaxLines());
    } else if (keys & Input::Key::Fire1) {
        if (filer->getSelection().type == Io::Type::Directory) {
            filer->enter(filer->getIndex());
        } else {
            // TODO
        }
    } else if (keys & Input::Key::Fire2) {
        filer->exit();
    }

    if (keys & EV_QUIT) {
        quit = true;
    }

    return C2DRenderer::onInput(players);
}

void RetroDream::onDraw(Transform &transform, bool draw) {

    // handle key repeat delay
    unsigned int keys = getInput()->getKeys(0);
    if (keys != Input::Key::Delay) {
        if (keys && timer.getElapsedTime().asSeconds() > 5) {
            getInput()->setRepeatDelay(INPUT_DELAY / 12);
        } else if (keys && timer.getElapsedTime().asSeconds() > 3) {
            getInput()->setRepeatDelay(INPUT_DELAY / 8);
        } else if (keys && timer.getElapsedTime().asSeconds() > 1) {
            getInput()->setRepeatDelay(INPUT_DELAY / 4);
        } else if (!keys) {
            getInput()->setRepeatDelay(INPUT_DELAY);
            timer.restart();
        }
    }

    C2DRenderer::onDraw(transform, draw);
}

int main(int argc, char **argv) {

    auto *retroDream = new RetroDream(Vector2f(C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT));
    retroDream->setClearColor(Color::Black);

    while (!retroDream->quit) {
        retroDream->flip();
    }

    delete (retroDream);

    return 0;
}
