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
#ifdef NDEBUG
KOS_INIT_FLAGS(INIT_DEFAULT | INIT_QUIET | INIT_NO_DCLOAD);
#else
KOS_INIT_FLAGS(INIT_DEFAULT);
#endif
#endif

using namespace c2d;

static RetroConfig *retroConfig = nullptr;
static c2d::Texture *splashTex = nullptr;

RetroDream::RetroDream(c2d::Renderer *r, const c2d::Vector2f &size, float outlineThickness)
        : RoundedRectangleShape(size, 10, 8) {

    render = r;
    setFillColor(COL_BLUE);
    setOutlineColor(COL_BLUE_DARK);
    setOutlineThickness(outlineThickness * 2);

#ifdef __PLATFORM_LINUX__
    Font *font = new Font();
    font->loadFromFile(render->getIo()->getDataPath() + "/future.ttf");
    render->setFont(font);
#endif
    render->getFont()->setFilter(Texture::Filter::Point);
    render->getFont()->setOffset({0, 5});

    debugClockStart("font cache");
    Text *cacheText = new Text(
            " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!:.,-_'()\"", FONT_SIZE);
    add(cacheText);
    render->flip(false, false);
    delete (cacheText);
    debugClockEnd("font cache");

    // statusBox, first
    statusBox = new StatusBox(this, {4, size.y - 4, size.x - 16, 40});
    statusBox->setOrigin(Origin::BottomLeft);
    add(statusBox);

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
    header->getTextLeft()->setFillColor(COL_YELLOW);
    header->getTextLeft()->setOutlineColor(Color::Black);
    header->getTextLeft()->setOutlineThickness(2);
    header->getTextRight()->setFillColor(COL_YELLOW);
    header->getTextRight()->setOutlineColor(Color::Black);
    header->getTextRight()->setOutlineThickness(2);
    add(header);

    /// splash/title texture
    add(splashTex);
    splashTex->setOrigin(Origin::Center);
    splashTex->setPosition(PERCENT(size.x, 76), PERCENT(size.y, 42));

    /// preview box
    float previewSize = (size.x / 2) - 32;
    FloatRect previewRect = {
            previewSize + 52, PERCENT(size.y, 10.0f),
            previewSize + 32, previewSize};
    preview = new Preview(previewRect);
    preview->setFillColor(COL_BLUE_GRAY);
    preview->setOutlineColor(COL_BLUE_DARK);
    preview->setOutlineThickness(3);
    add(preview);

    helpBox = new HelpBox(this, {previewRect.left,
                                 previewRect.top + previewRect.height + 8,
                                 previewSize, PERCENT(size.y, 22.2f)});
    helpBox->setFillColor(COL_BLUE_LIGHT);
    helpBox->setOutlineColor(COL_BLUE_DARK);
    helpBox->setOutlineThickness(2);
    add(helpBox);

    /// filers
    FloatRect filerRect = {
            PERCENT(size.x, 1.5f), PERCENT(size.y, 10.0f),
            PERCENT(size.x, 50.0f), PERCENT(size.y, 79.0f)
    };
    filerLeft = new Filer(this, filerRect, retroConfig->get(RetroConfig::FilerPath), 10);
    filerLeft->setFillColor(COL_BLUE_GRAY);
    filerLeft->setOutlineColor(COL_BLUE_DARK);
    filerLeft->setOutlineThickness(3);
    filerLeft->setColor(COL_BLUE_DARK, COL_BLUE);
    add(filerLeft);
    filer = filerLeft;

    // "hide main rect layer"
    blurLayer = new RectangleShape(render->getSize());
    blurLayer->setPosition(-outlineThickness, -outlineThickness);
    blurLayer->setFillColor(Color::Gray);
    blurLayer->add(new TweenAlpha(0, 230, 0.3));
    blurLayer->setVisibility(Visibility::Hidden);
    add(blurLayer);

    fileMenu = new FileMenu(this, previewRect);
    fileMenu->setFillColor(COL_BLUE_GRAY);
    fileMenu->setOutlineColor(COL_BLUE_DARK);
    fileMenu->setOutlineThickness(3);
    add(fileMenu);

    FloatRect optionMenuRect = {
            size.x / 2, size.y,
            PERCENT(size.x, 60), PERCENT(size.y, 70)
    };
    optionMenu = new OptionMenu(this, optionMenuRect);
    optionMenu->setOrigin(Origin::Bottom);
    optionMenu->setFillColor(COL_BLUE_GRAY);
    optionMenu->setOutlineColor(COL_BLUE_DARK);
    optionMenu->setOutlineThickness(3);
    add(optionMenu);

    FloatRect CreditsRect = {
            size.x / 2, size.y / 2,
            PERCENT(size.x, 75), PERCENT(size.y, 75)
    };
    credits = new Credits(this, CreditsRect);
    credits->setOrigin(Origin::Center);
    credits->setFillColor(COL_BLUE_GRAY);
    credits->setOutlineColor(COL_BLUE_DARK);
    credits->setOutlineThickness(3);
    add(credits);

    inputDelay = retroConfig->getInt(RetroConfig::InputDelay);
    render->getInput()->setRepeatDelay(inputDelay);
    timer.restart();
}

bool RetroDream::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (!credits->isVisible()) {
        if ((keys & Input::Key::Fire3) && (keys & Input::Key::Fire5) && (keys & Input::Key::Fire6)) {
            std::string path = retroConfig->get(RetroConfig::RdPath) + "screenshots/";
            RetroUtility::screenshot(this, path);
        } else if (keys & Input::Key::Fire4) {
            Filer::RetroFile file = filer->getSelection();
            if (file.isGame) {
                optionMenu->setVisibility(Visibility::Hidden, true);
                fileMenu->setTitle(file.isGame ? "LOADER OPTIONS" : "FILE OPTIONS");
                fileMenu->setVisibility(fileMenu->isVisible() ?
                                        Visibility::Hidden : Visibility::Visible, true);
                fileMenu->save();
                blurLayer->setVisibility(fileMenu->getVisibility(), true);
            }
        } else if (keys & Input::Key::Start) {
            optionMenu->setVisibility(optionMenu->isVisible() ?
                                      Visibility::Hidden : Visibility::Visible, true);
            fileMenu->setVisibility(Visibility::Hidden, true);
            fileMenu->save();
            blurLayer->setVisibility(optionMenu->getVisibility(), true);
        }
    }

    if (keys & EV_QUIT) {
        quit = true;
    }

    return C2DObject::onInput(players);
}

void RetroDream::onUpdate() {

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

    RectangleShape::onUpdate();
}

RetroConfig *RetroDream::getConfig() {
    return retroConfig;
}

void RetroDream::showStatus(const std::string &title, const std::string &msg,
                            const c2d::Color &color) {
    statusBox->show(title, msg, color);
}

void RetroDream::debugClockStart(const char *msg) {
#ifndef NDEBUG
    printf("debugClockStart: %s\n", msg);
    debugClock.restart();
#endif
}

void RetroDream::debugClockEnd(const char *msg) {
#ifndef NDEBUG
    printf("debugClockEnd: %s: %f\n", msg, debugClock.getElapsedTime().asSeconds());
#endif
}

int main() {

    c2d_default_font_texture_size = {512, 512};

    /// render
    auto render = new C2DRenderer({C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});

    /// splash
    auto splash = new C2DRectangle({0, 0, C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});
    splash->setFillColor(Color::White);
    render->add(splash);
    splashTex = new C2DTexture(render->getIo()->getRomFsPath() + "skin/splash.png");
    auto sprite = new Sprite(splashTex, {0, 0, splashTex->getSize().x, splashTex->getSize().y});
    sprite->setOrigin(Origin::Center);
    sprite->setPosition((float) C2D_SCREEN_WIDTH / 2, (float) C2D_SCREEN_HEIGHT / 2);
    render->add(sprite);
    render->flip();
    delete (sprite);
    /// splash

#ifdef __DREAMCAST__
#ifdef NDEBUG
    InitSDCard();
#endif
    InitIDE();
#endif

    /// config
    auto retroIo = new RetroIo();
    retroConfig = new RetroConfig(retroIo);
    render->setIo(retroIo);

    /// main rect
    FloatRect screenSize = retroConfig->getRect(RetroConfig::ScreenSize);
    //FloatRect screenSize = {32, 32, 640-64, 480-64};
    float outline = 6;
    FloatRect rect = {screenSize.left + outline, screenSize.top + outline,
                      screenSize.width - (outline * 2), screenSize.height - (outline * 2)};
    auto *retroDream = new RetroDream(render, {rect.width, rect.height}, outline);
    retroDream->setPosition(rect.left, rect.top);
    render->add(retroDream);

    // let's go
    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

    return 0;
}
