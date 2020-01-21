//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "utility.h"
#include "isoloader.h"
#include "retroio.h"

#ifdef __DREAMCAST__
extern "C" {
#include "ds/include/fs.h"
#include "ds/include/isoldr.h"
}
#endif

using namespace c2d;

RetroConfig *retroConfig;

RetroDream::RetroDream(c2d::Renderer *r, const c2d::Vector2f &size) : RectangleShape(size) {

    render = r;

#ifdef __PLATFORM_LINUX__
    Font *font = new Font();
    font->loadFromFile(render->getIo()->getDataPath() + "/m23.ttf");
    render->setFont(font);
#endif
    render->getFont()->setOffset({0, 4});

    setFillColor(COL_BLUE);
    setOutlineColor(COL_BLUE_DARK);
    setOutlineThickness(4);
    setPosition(4, 4);

    header = new Header({size.x - 16, 32}, 10, 8);
    header->setPosition(8, 8);
    header->setFillColor(COL_BLUE_DARK);
    header->setOutlineColor(Color::White);
    header->setOutlineThickness(2);
    header->getText()->setFillColor(COL_YELLOW);
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
    filerLeft = new Filer(this, filerRect, retroConfig->getLastPath());
    filerLeft->setFillColor(COL_BLUE_GRAY);
    filerLeft->setOutlineColor(Color::White);
    filerLeft->setOutlineThickness(2);
    add(filerLeft);

    /*
    filerRect = {(size.x / 2) + 4, 64, (size.x / 2) - 10, size.y - (64 + 16)};
    filerRight = new Filer(this, filerRect, "/");
    filerRight->setFillColor(COL_BLUE_GRAY);
    filerRight->setOutlineColor(Color::White);
    filerRight->setOutlineThickness(2);
    filerRight->setAlpha(100);
    filerRight->setVisibility(Visibility::Hidden);
    add(filerRight);
    */

    filer = filerLeft;

    debugMessage = new Text("DEBUG: ", 16);
    debugMessage->setPosition(16, 440);
    debugMessage->setFillColor(COL_RED);
    debugMessage->setOutlineColor(Color::Black);
    debugMessage->setOutlineThickness(1);
    add(debugMessage);

    render->getInput()->setRepeatDelay(retroConfig->getInputDelay());
    timer.restart();
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
        Io::Type type = filer->getSelection().data.type;
        if (filer->getSelection().isGame) {
            run_iso(filer->getSelection().isoPath.c_str());
        } else if (type == Io::Type::File && RetroUtility::isElf(filer->getSelection().data.name)) {
            RetroUtility::exec(filer->getSelection().data.path);
        } else if (type == Io::Type::Directory) {
            filer->enter(filer->getIndex());
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
    unsigned int keys = render->getInput()->getKeys(0);

    if (keys != Input::Key::Delay) {
        bool changed = (oldKeys ^ keys) != 0;
        oldKeys = keys;
        if (!changed) {
            if (timer.getElapsedTime().asSeconds() > 5) {
                render->getInput()->setRepeatDelay(retroConfig->getInputDelay() / 12);
            } else if (timer.getElapsedTime().asSeconds() > 3) {
                render->getInput()->setRepeatDelay(retroConfig->getInputDelay() / 8);
            } else if (timer.getElapsedTime().asSeconds() > 1) {
                render->getInput()->setRepeatDelay(retroConfig->getInputDelay() / 4);
            }
        } else {
            render->getInput()->setRepeatDelay(retroConfig->getInputDelay());
            timer.restart();
        }
    }

    RectangleShape::onDraw(transform, draw);
}

c2d::Renderer *RetroDream::getRender() {
    return render;
}

Header *RetroDream::getHeader() {
    return header;
}

Preview *RetroDream::getPreview() {
    return preview;
}

Filer *RetroDream::getFiler() {
    return filer;
}

RetroConfig *RetroDream::getConfig() {
    return retroConfig;
}

int main(int argc, char **argv) {

    dbgio_dev_select("scif");

#ifdef __DREAMCAST__
    //InitSDCard();
    InitIDE();
#endif

    auto retroIo = new RetroIo();
    retroConfig = new RetroConfig(retroIo);
    printf("data_path: %s\n", retroConfig->getDataPath().c_str());
    printf("last_path: %s\n", retroConfig->getLastPath().c_str());

    auto *render = new C2DRenderer({retroConfig->getScreenSize().width, retroConfig->getScreenSize().height});
    render->setPosition(retroConfig->getScreenSize().left, retroConfig->getScreenSize().top);
    render->setIo(retroIo);

    auto *retroDream = new RetroDream(render, {render->getSize().x - 8, render->getSize().y - 8});
    render->add(retroDream);

    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

    return 0;
}
