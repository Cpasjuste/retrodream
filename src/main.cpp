//
// Created by cpasjuste on 15/01/2020.
//

#ifndef __DREAMCAST__

#include <zconf.h>

#endif

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "utility.h"
#include "isoloader.h"
#include "retroio.h"
#include "systemmenu.h"
#include "skintex.h"

#ifdef __DREAMCAST__
extern "C" {
#include "ds/include/fs.h"
#include "ds/include/isoldr.h"
void vmu_draw_str(uint8 bitmap[192], unsigned char *str, int x, int y);
}
#endif

using namespace c2d;

static Renderer *render = nullptr;
static Texture *splashTex = nullptr;
static Sprite *splashSprite = nullptr;
static Text *debugText = nullptr;
static RetroConfig *retroConfig = nullptr;
static Skin *skin = nullptr;

RetroDream::RetroDream(c2d::Renderer *r, Skin::CustomShape *_shape) : SkinRect(_shape) {

    render = r;
    Vector2f size = SkinRect::getSize();

#if defined( __LINUX__) || defined(__WINDOWS__)
    Font *font = new Font();
    font->loadFromFile(render->getIo()->getRomFsPath() + "skin/future.ttf");
    render->setFont(font);
    debugText->setFont(font);
#endif

    retroDebug("LOADING FONT...");
    debugClockStart("font cache");
    Text *cacheText = new Text(
            " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!:.,-_'()/\"", FONT_SIZE);
    cacheText->setOutlineColor(Color::Black);
    cacheText->setOutlineThickness(2);
    render->add(cacheText);
    render->flip(false, false);
    delete (cacheText);
    debugClockEnd("font cache");

    /// header text
    retroDebug("LOADING HEADER BOX...");
    Skin::CustomShape shape = skin->getShape(Skin::Id::FilerPathShape);
    header = new Header(&shape);
    Shape::add(header);

    /// splash/title texture
    shape = skin->getShape(Skin::Id::LogoShape);
    splashTex->setOrigin(shape.origin);
    splashTex->setPosition(shape.rect.left, shape.rect.top);
    splashTex->setSize(shape.rect.width, shape.rect.height);
    splashTex->setFillColor(shape.color);
    splashTex->setOutlineColor(shape.outlineColor);
    splashTex->setOutlineThickness(shape.outlineSize);
    Shape::add(splashTex);

    retroDebug("LOADING HELP BOX...");
    shape = skin->getShape(Skin::Id::HelpShape);
    helpBox = new HelpBox(this, &shape);
    Shape::add(helpBox);

    /// preview box
    retroDebug("LOADING PREVIEW IMAGE BOX...");
    shape = skin->getShape(Skin::Id::PreviewImageShape);
    preview = new Preview(&shape);
    Shape::add(preview);

    /// preview box (video)
    retroDebug("LOADING PREVIEW VIDEO BOX...");
    shape = skin->getShape(Skin::Id::PreviewVideoShape);
    previewVideo = new PreviewVideo(this, &shape);
    Shape::add(previewVideo);

    /// filers
    retroDebug("LOADING GAMES...");
    shape = skin->getShape(Skin::Id::FilerShape);
    filer = new Filer(this, &shape, retroConfig->get(RetroConfig::FilerPath), 10);
    Shape::add(filer);

    retroDebug("LOADING GAME MENU...");
    shape = skin->getShape(Skin::Id::MenuShape);
    presetMenu = new PresetMenu(this, &shape);
    Shape::add(presetMenu);

    retroDebug("LOADING FILE MENU...");
    fileMenu = new FileMenu(this, &shape);
    Shape::add(fileMenu);

    retroDebug("LOADING SYSTEM MENU...");
    systemMenu = new SystemMenu(this, &shape);
    Shape::add(systemMenu);

    retroDebug("LOADING REGION FREE MENU...");
    regionFreeMenu = new RegionFreeMenu(this, &shape);
    Shape::add(regionFreeMenu);

    retroDebug("LOADING OPTIONS MENU...");
    optionMenu = new OptionMenu(this, &shape);
    Shape::add(optionMenu);

    retroDebug("LOADING CREDITS...");
    FloatRect CreditsRect = {
            size.x / 2, size.y / 2,
            PERCENT(size.x, 75), PERCENT(size.y, 75)
    };
    credits = new Credits(this, CreditsRect);
    credits->setOrigin(Origin::Center);
    credits->setFillColor(COL_BLUE_GRAY);
    credits->setOutlineColor(COL_BLUE_DARK);
    credits->setOutlineThickness(3);
    Shape::add(credits);

    progressBox = new ProgressBox(this, COL_BLUE_LIGHT, COL_BLUE_DARK, COL_BLUE_DARK);
    progressBox->getTitle()->setFillColor(COL_RED);
    Shape::add(progressBox);

    messageBox = new MessageBox(progressBox->getLocalBounds(), render->getInput(), render->getFont(), FONT_SIZE);
    messageBox->setPosition(progressBox->getPosition());
    messageBox->setOrigin(Origin::Center);
    messageBox->setFillColor(progressBox->getFillColor());
    messageBox->setOutlineColor(progressBox->getOutlineColor());
    messageBox->setOutlineThickness(progressBox->getOutlineThickness());
    messageBox->setSelectedColor(COL_BLUE_DARK, COL_RED);
    messageBox->setNotSelectedColor(COL_BLUE_DARK, Color::White);
    messageBox->getTitleText()->setFillColor(COL_RED);
    messageBox->getMessageText()->setFillColor(COL_BLUE_DARK);
    Shape::add(messageBox);

    statusBox = new StatusBox({8, size.y - 14, size.x, FONT_SIZE * 2});
    statusBox->setOrigin(Origin::BottomLeft);
    Shape::add(statusBox);

    inputDelay = retroConfig->getInt(RetroConfig::InputDelay);
    render->getInput()->setRepeatDelay(inputDelay);
    timer.restart();

    retroDebug("ALMOST DONE...");
}

bool RetroDream::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (systemMenu->isVisible() || regionFreeMenu->isVisible() || credits->isVisible()
        || progressBox->isVisible() || messageBox->isVisible()) {
        return C2DObject::onInput(players);
    }

    if ((keys & Input::Key::Fire3) && (keys & Input::Key::Fire5) && (keys & Input::Key::Fire6)) {
        RetroUtility::screenshot(this, getIo()->getScreenshotPath());
    } else if (keys & Input::Key::Fire3 && !optionMenu->isVisible()) {
        optionMenu->setVisibility(Visibility::Hidden, true);
        presetMenu->setVisibility(Visibility::Hidden, true);
        fileMenu->setVisibility(fileMenu->isVisible() ?
                                Visibility::Hidden : Visibility::Visible, true);
    } else if (keys & Input::Key::Fire4 && !optionMenu->isVisible()) {
        Filer::RetroFile file = filer->getSelection();
        if (file.isGame) {
            optionMenu->setVisibility(Visibility::Hidden, true);
            fileMenu->setVisibility(Visibility::Hidden, true);
            presetMenu->setVisibility(presetMenu->isVisible() ?
                                      Visibility::Hidden : Visibility::Visible, true);
        }
    } else if (keys & Input::Key::Start) {
        fileMenu->setVisibility(Visibility::Hidden, true);
        presetMenu->setVisibility(Visibility::Hidden, true);
        optionMenu->setVisibility(optionMenu->isVisible() ?
                                  Visibility::Hidden : Visibility::Visible, true);
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

Skin *RetroDream::getSkin() {
    return skin;
}

void RetroDream::showStatus(const std::string &title, const std::string &msg, const c2d::Color &color) {
#if 0
    if (statusBox != nullptr) {
        statusBox->show(title, msg, color);
    }
#endif
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

void retroDebug(const char *fmt, ...) {

    if (debugText != nullptr) {
        va_list args;
        char buffer[512];

        memset(buffer, 0, 512);
        va_start(args, fmt);
        vsnprintf(buffer, MAX_PATH, fmt, args);
        va_end(args);

        debugText->setString(Utility::toUpper(buffer));
        debugText->setVisibility(Visibility::Visible);
        render->flip();
    }
}

int main(int argc, char *argv[]) {

    c2d_default_font_texture_size = {256, 256};

    /// render
    render = new C2DRenderer({C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});

#ifdef __DREAMCAST__
    // vmu
    uint8 bitmap[192];
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_LCD);
    if (vmu != nullptr) {
        memset(bitmap, 0, sizeof(bitmap));
        vmu_draw_str(bitmap, (uint8 *) "Retro", 12, 0);
        vmu_draw_str(bitmap, (uint8 *) "Dream", 12, 10);
        std::string ver = std::string(VERSION_MAJOR) + "."
                          + std::string(VERSION_MINOR) + "."
                          + std::string(VERSION_MICRO);
        vmu_draw_str(bitmap, (uint8 *) ver.c_str(), 12, 20);
        vmu_draw_lcd(vmu, bitmap);
    }
#endif

    /// splash
    auto splash = new C2DRectangle({0, 0, C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT});
    splash->setFillColor(Color::White);
    render->add(splash);
    splashTex = new C2DTexture(render->getIo()->getRomFsPath() + "skin/splash.png");
    splashSprite = new Sprite(splashTex, {0, 0, splashTex->getSize().x, splashTex->getSize().y});
    splashSprite->setOrigin(Origin::Center);
    splashSprite->setPosition((float) C2D_SCREEN_WIDTH / 2, (float) C2D_SCREEN_HEIGHT / 2);
    render->add(splashSprite);
    render->flip();
    /// splash

    /// debug
    debugText = new Text("LOADING...", FONT_SIZE);
    debugText->setFillColor(COL_BLUE_DARK);
    debugText->setOutlineColor(Color::Black);
    debugText->setOutlineThickness(2);
    debugText->setOrigin(Origin::BottomLeft);
    debugText->setPosition(16, C2D_SCREEN_HEIGHT - 16);
    render->add(debugText);

#ifdef __DREAMCAST__
    retroDebug("MOUNTING HDD...");
    InitIDE();
#ifdef NDEBUG
    retroDebug("MOUNTING SDCARD...");
    InitSDCard();
#endif
#ifdef __EMBEDDED_MODULE_DEBUG__
    fs_iso_init();
#endif
#endif

    /// config
    retroDebug("LOADING CONFIG...");
    auto retroIo = new RetroIo();
    retroConfig = new RetroConfig(retroIo);
    render->setIo(retroIo);

    /// skin
    skin = new Skin(retroIo);

    /// main rect
    Skin::CustomShape shape = skin->getShape(Skin::Id::BackgroundShape);
    auto *retroDream = new RetroDream(render, &shape);
    render->add(retroDream);

    // be sure all stuff is updated/created before splash deletion
    render->flip();
    delete (splashSprite);
    delete (debugText);
    debugText = nullptr;

#ifdef __DREAMCAST__
    cdrom_spin_down();
#endif

    // let's go
    while (!retroDream->quit) {
        render->flip();
    }

    delete (render);

#ifdef __DREAMCAST__
#ifdef __EMBEDDED_MODULE_DEBUG__
    fs_iso_shutdown();
#endif
#endif

    return 0;
}
