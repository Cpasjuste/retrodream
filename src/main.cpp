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
        : RoundedRectangleShape(size, 10, 8) {

    render = r;

#ifdef __PLATFORM_LINUX__
    Font *font = new Font();
    font->loadFromFile(render->getIo()->getDataPath() + "/future.ttf");
    render->setFont(font);
#endif
    render->getFont()->setFilter(Texture::Filter::Point);
    render->getFont()->setOffset({0, 5});

    setFillColor(COL_BLUE);
    setOutlineColor(COL_BLUE_DARK);
    setOutlineThickness(outlineThickness + 6);

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

    /// filers
    FloatRect filerRect = {
            PERCENT(size.x, 1.5f), PERCENT(size.y, 10.0f),
            PERCENT(size.x, 50.0f), PERCENT(size.y, 79.0f)
    };
    filerLeft = new Filer(this, filerRect, retroConfig->get(RetroConfig::LastPath), 12);
    filerLeft->setFillColor(COL_BLUE_GRAY);
    filerLeft->setOutlineColor(COL_BLUE_DARK);
    filerLeft->setOutlineThickness(3);
    filerLeft->setColor(COL_BLUE_DARK, COL_BLUE);
    add(filerLeft);
    filer = filerLeft;

    /// preview box
    float previewSize = (size.x / 2) - 32;
    preview = new Preview({previewSize, previewSize});
    preview->setPosition(previewSize + 52, PERCENT(size.y, 10.0f));
    preview->setFillColor(COL_BLUE_DARK);
    preview->setOutlineColor(Color::White);
    preview->setOutlineThickness(3);
    add(preview);

    infoBox = new InfoBox({preview->getPosition().x,
                           preview->getPosition().y + preview->getSize().y + 8,
                           preview->getSize().x, PERCENT(size.y, 22.2f)});
    infoBox->setFillColor(COL_BLUE_GRAY);
    infoBox->setOutlineColor(COL_BLUE_DARK);
    infoBox->setOutlineThickness(2);
    infoBox->text->setFillColor(COL_BLUE_DARK);
    add(infoBox);

    statusBox = new StatusBox(this, {4, size.y - 4, size.x - 16, 40});
    statusBox->setOrigin(Origin::BottomLeft);
    add(statusBox);

    // "hide main rect layer"
    blurLayer = new RectangleShape(render->getSize());
    blurLayer->setPosition(-outlineThickness, -outlineThickness);
    blurLayer->setFillColor(Color::Gray);
    blurLayer->add(new TweenAlpha(0, 230, 0.3));
    blurLayer->setVisibility(Visibility::Hidden);
    add(blurLayer);

    FloatRect fileMenuRect = {
            previewSize + 52, PERCENT(size.y, 10.0f),
            previewSize + 32, previewSize};
    fileMenu = new FileMenu(this, fileMenuRect);
    fileMenu->setFillColor(COL_BLUE);
    fileMenu->setOutlineColor(COL_BLUE_DARK);
    fileMenu->setOutlineThickness(6);
    add(fileMenu);

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

    inputDelay = retroConfig->getInt(RetroConfig::InputDelay);
    render->getInput()->setRepeatDelay(inputDelay);
    timer.restart();
}

bool RetroDream::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire4) {
        Filer::RetroFile file = filer->getSelection();
        optionMenu->setVisibility(Visibility::Hidden, true);
        fileMenu->setTitle(file.isGame ? "LOADER OPTIONS" : "FILE OPTIONS");
        fileMenu->setVisibility(fileMenu->isVisible() ?
                                Visibility::Hidden : Visibility::Visible, true);
        fileMenu->save();
        blurLayer->setVisibility(fileMenu->getVisibility(), true);
    } else if (keys & Input::Key::Start) {
        optionMenu->setVisibility(optionMenu->isVisible() ?
                                  Visibility::Hidden : Visibility::Visible, true);
        fileMenu->setVisibility(Visibility::Hidden, true);
        fileMenu->save();
        blurLayer->setVisibility(optionMenu->getVisibility(), true);
    }

    if (keys & EV_QUIT) {
        quit = true;
    }

    return C2DObject::onInput(players);
}

void RetroDream::onDraw(Transform &transform, bool draw) {

    // handle key repeat delay
    unsigned int keys = render->getInput()->getKeys(0);

    if (keys != Input::Key::Delay) {
        bool changed = (oldKeys ^ keys) != 0;
        oldKeys = keys;
        if (!changed) {
            if (timer.getElapsedTime().asSeconds() > 5) {
                render->getInput()->setRepeatDelay(inputDelay / 12);
            } else if (timer.getElapsedTime().asSeconds() > 3) {
                render->getInput()->setRepeatDelay(inputDelay / 8);
            } else if (timer.getElapsedTime().asSeconds() > 1) {
                render->getInput()->setRepeatDelay(inputDelay / 4);
            }
        } else {
            render->getInput()->setRepeatDelay(inputDelay);
            timer.restart();
        }
    }

    RectangleShape::onDraw(transform, draw);
}

RetroConfig *RetroDream::getConfig() {
    return retroConfig;
}

void RetroDream::showStatus(const std::string &title, const std::string &msg) {
    if (statusBox != nullptr) {
        statusBox->show(title, msg);
    }
}

int main() {

#ifdef __DREAMCAST__
#ifdef NDEBUG
    InitSDCard();
#endif
    InitIDE();
#ifdef __EMBEDDED_MODULE_DEBUG__
    fs_iso_init();
#endif
#endif

    /// config
    auto retroIo = new RetroIo();
    retroConfig = new RetroConfig(retroIo);
    printf("data_path: %s\n", retroConfig->get(RetroConfig::DataPath).c_str());
    printf("last_path: %s\n", retroConfig->get(RetroConfig::LastPath).c_str());

    /// render
    auto *render = new C2DRenderer({C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});
    render->setIo(retroIo);

    /// main rect
    FloatRect screenSize = retroConfig->getRect(RetroConfig::ScreenSize);

    // splash
    RectangleShape *splashRect = new RectangleShape(screenSize);
    splashRect->setFillColor(COL_BLUE_GRAY);
    C2DTexture *splashTex = new C2DTexture(retroIo->getRomFsPath() + "skin/splash.png");
    splashTex->setOrigin(Origin::Center);
    splashTex->setPosition(screenSize.width / 2, screenSize.height / 2);
    splashRect->add(splashTex);
    render->add(splashRect);
    for (int i = 0; i < 5; i++) {
        render->flip();
    }

    /// main rect
    float outline = 6;
    FloatRect rect = {screenSize.left + outline, screenSize.top + outline,
                      screenSize.width - outline * 2, screenSize.height - outline * 2};
    auto *retroDream = new RetroDream(render, {rect.width, rect.height}, outline);
    retroDream->setPosition(rect.left, rect.top);
    render->add(retroDream);

    // delete splash
    delete (splashRect);

    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

#if defined(__DREAMCAST__) && defined (__EMBEDDED_MODULE_DEBUG__)
    fs_iso_shutdown();
#endif

    return 0;
}
