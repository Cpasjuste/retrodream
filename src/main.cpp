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

RetroDream::RetroDream(c2d::Renderer *r, const c2d::Vector2f &size, float outlineThickness)
        : RectangleShape(size) {

    render = r;

#ifdef __PLATFORM_LINUX__
    Font *font = new Font();
    font->loadFromFile(render->getIo()->getDataPath() + "/magnum.ttf");
    render->setFont(font);
#endif
    render->getFont()->setFilter(Texture::Filter::Point);
    render->getFont()->setOffset({0, -2});

    setFillColor(COL_BLUE);
    setOutlineColor(COL_BLUE_DARK);
    setOutlineThickness(outlineThickness);

    /// header text
    FloatRect headerRect = {
            PERCENT(size.x, 1.5f), PERCENT(size.y, 1.5f),
            PERCENT(size.x, 97), PERCENT(size.y, 6.5f)
    };
    header = new Header(headerRect);
    header->setPosition(8, 8);
    header->setFillColor(COL_BLUE_DARK);
    header->setOutlineColor(Color::White);
    header->setOutlineThickness(2);
    header->getText()->setFillColor(COL_YELLOW);
    header->getText()->setOutlineColor(Color::Black);
    header->getText()->setOutlineThickness(2);
    add(header);

    /// preview box
    float previewSize = (size.x / 2) - 20;
    preview = new Preview({previewSize, previewSize});
    preview->setPosition(previewSize + 30, 48);
    preview->setFillColor(COL_BLUE_DARK);
    preview->setOutlineColor(Color::White);
    preview->setOutlineThickness(2);
    add(preview);

    /// filers
    FloatRect filerRect = {8, 48, (size.x / 2) - 10, size.y - 96};
    filerLeft = new Filer(this, filerRect, retroConfig->getLastPath());
    filerLeft->setFillColor(COL_BLUE_GRAY);
    filerLeft->setOutlineColor(Color::White);
    filerLeft->setOutlineThickness(2);
    filerLeft->setColor(COL_BLUE_DARK, COL_BLUE);
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

    // "hide main rect layer"
    blurLayer = new RectangleShape(render->getSize());
    blurLayer->setPosition(-outlineThickness, -outlineThickness);
    blurLayer->setFillColor(Color::Gray);
    blurLayer->add(new TweenAlpha(0, 200, 0.3));
    blurLayer->setVisibility(Visibility::Hidden);
    add(blurLayer);

    FloatRect optionMenuRect = {
            size.x / 2, size.y,
            PERCENT(size.x, 80), PERCENT(size.y, 70)
    };
    optionMenu = new OptionMenu(this, optionMenuRect);
    optionMenu->setOrigin(Origin::Bottom);
    optionMenu->setFillColor(COL_BLUE);
    optionMenu->setOutlineColor(COL_BLUE_DARK);
    optionMenu->setOutlineThickness(6);
    add(optionMenu);

    FloatRect fileMenuRect = {
            0, size.y / 2,
            PERCENT(size.x, 40), PERCENT(size.y, 70)
    };
    fileMenu = new FileMenu(this, fileMenuRect);
    fileMenu->setOrigin(Origin::Left);
    fileMenu->setFillColor(COL_BLUE);
    fileMenu->setOutlineColor(COL_BLUE_DARK);
    fileMenu->setOutlineThickness(6);
    add(fileMenu);

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
            IsoLoader::run(render->getIo(), filer->getSelection().isoPath);
        } else if (type == Io::Type::File && RetroUtility::isElf(filer->getSelection().data.name)) {
            RetroUtility::exec(filer->getSelection().data.path);
        } else if (type == Io::Type::Directory) {
            filer->enter(filer->getIndex());
        }
    } else if (keys & Input::Key::Fire2) {
        filer->exit();
    } else if (keys & Input::Key::Fire3) {
        fileMenu->setVisibility(fileMenu->isVisible() ?
                                Visibility::Hidden : Visibility::Visible, true);
        blurLayer->setVisibility(fileMenu->getVisibility(), true);
    } else if (keys & Input::Key::Fire4) {
    } else if (keys & Input::Key::Start) {
        optionMenu->setVisibility(optionMenu->isVisible() ?
                                  Visibility::Hidden : Visibility::Visible, true);
        blurLayer->setVisibility(optionMenu->getVisibility(), true);
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

RetroConfig *RetroDream::getConfig() {
    return retroConfig;
}

int main() {

#ifdef __DREAMCAST__
#ifdef NDEBUG
    InitSDCard();
#endif
    InitIDE();
#endif

    /// config
    auto retroIo = new RetroIo();
    retroConfig = new RetroConfig(retroIo);
    printf("data_path: %s\n", retroConfig->getDataPath().c_str());
    printf("last_path: %s\n", retroConfig->getLastPath().c_str());

    /// render
    auto *render = new C2DRenderer({C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});
    render->setIo(retroIo);

#if defined(__DREAMCAST__) && !defined(NDEBUG)
    // dc-load "recovery"
    // flip renderer to update inputs
    render->flip();
    if (render->getInput()->getKeys() & Input::Key::Start) {
        RetroUtility::exec("/rd/dcload-serial.bin");
    }
#endif

    /// main rect
    float outline = 6;
    FloatRect rect = {retroConfig->getScreenSize().left + outline,
                      retroConfig->getScreenSize().top + outline,
                      retroConfig->getScreenSize().width - outline * 2,
                      retroConfig->getScreenSize().height - outline * 2};
    auto *retroDream = new RetroDream(render, {rect.width, rect.height}, outline);
    retroDream->setPosition(rect.left, rect.top);
    render->add(retroDream);

    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

    return 0;
}
