//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "utility.h"

using namespace c2d;

RetroDream::RetroDream(c2d::Renderer *r, const c2d::Vector2f &size) : RectangleShape(size) {

    renderer = r;
#ifdef __PLATFORM_LINUX__
    Font *font = new Font();
    font->loadFromFile(renderer->getIo()->getDataPath() + "/m23.ttf");
    renderer->setFont(font);
#endif
    renderer->getFont()->setOffset({0, 4});

    header = new Header({size.x - 16, 32}, 10, 8);
    header->setPosition(8, 8);
    header->setFillColor(COL_BLUE_DARK);
    header->setOutlineColor(Color::White);
    header->setOutlineThickness(2);
    header->getText()->setFillColor(COL_BLUE_GRAY);
    header->getText()->setOutlineColor(Color::Black);
    header->getText()->setOutlineThickness(2);
    add(header);

    float previewSize = (size.x / 2) - 20;
    preview = new Preview({previewSize, previewSize}, 10, 8);
    preview->setPosition(previewSize + 30, 48);
    preview->setFillColor(COL_BLUE_DARK);
    preview->setOutlineColor(Color::White);
    preview->setOutlineThickness(2);
    add(preview);

    FloatRect filerRect = {8, 48, (size.x / 2) - 10, size.y - 96};
    filerLeft = new Filer(this, filerRect, "/home/cpasjuste/dev/dreamcast/games");
    filerLeft->setFillColor(COL_BLUE_GRAY);
    filerLeft->setOutlineColor(Color::White);
    filerLeft->setOutlineThickness(2);
    add(filerLeft);

    filerRect = {(size.x / 2) + 4, 64, (size.x / 2) - 10, size.y - (64 + 16)};
    filerRight = new Filer(this, filerRect, "/home/cpasjuste/dev/dreamcast/games");
    filerRight->setFillColor(COL_BLUE_GRAY);
    filerRight->setOutlineColor(Color::White);
    filerRight->setOutlineThickness(2);
    filerRight->setAlpha(100);
    filerRight->setVisibility(Visibility::Hidden);
    add(filerRight);

    filer = filerLeft;
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
            Io::File file = filer->getSelection();
            if (Utility::endsWith(file.name, ".elf", false)
                || Utility::endsWith(file.name, ".bin", false)) {
                RetroUtility::exec(file.path);
            }
        }
    } else if (keys & Input::Key::Fire2) {
        filer->exit();
    } else if (keys & Input::Key::Fire5) {
        /*
        if (filer == filerLeft) {
            filer->setAlpha(100);
            filer = filerRight;
            filer->setAlpha(255);
        }
        */
    } else if (keys & Input::Key::Fire6) {
        /*
        if (filer == filerRight) {
            filer->setAlpha(100);
            filer = filerLeft;
            filer->setAlpha(255);
        }
        */
    }

    if (keys & EV_QUIT) {
        quit = true;
    }

    return RectangleShape::onInput(players);
}

void RetroDream::onDraw(Transform &transform, bool draw) {

    // handle key repeat delay
    unsigned int keys = renderer->getInput()->getKeys(0);
    if (keys != Input::Key::Delay) {
        if (keys > 0 && timer.getElapsedTime().asSeconds() > 5) {
            renderer->getInput()->setRepeatDelay(INPUT_DELAY / 12);
        } else if (keys > 0 && timer.getElapsedTime().asSeconds() > 3) {
            renderer->getInput()->setRepeatDelay(INPUT_DELAY / 8);
        } else if (keys > 0 && timer.getElapsedTime().asSeconds() > 1) {
            renderer->getInput()->setRepeatDelay(INPUT_DELAY / 4);
        } else if (keys < 1) {
            renderer->getInput()->setRepeatDelay(INPUT_DELAY);
            timer.restart();
        }
    }

    RectangleShape::onDraw(transform, draw);
}

c2d::Renderer *RetroDream::getRender() {
    return renderer;
}

Header *RetroDream::getHeader() {
    return header;
}

Preview *RetroDream::getPreview() {
    return preview;
}

RetroDream::~RetroDream() {

}

int main() {

    auto *render = new C2DRenderer({640, 480});
    render->setClearColor(Color::Black);

    auto *retroDream = new RetroDream(render, {render->getSize().x - 8, render->getSize().y - 8});
    retroDream->setFillColor(COL_BLUE);
    retroDream->setOutlineColor(COL_BLUE_DARK);
    retroDream->setOutlineThickness(4);
    retroDream->setPosition(4, 4);
    render->add(retroDream);

    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

    return 0;
}
