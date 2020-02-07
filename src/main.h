//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_MAIN_H
#define RETRODREAM_MAIN_H

#include "colors.h"
#include "cfg.h"
#include "filer.h"
#include "header.h"
#include "preview.h"
#include "optionmenu.h"
#include "filemenu.h"
#include "statusbox.h"
#include "utility.h"
#include "helpbox.h"
#include "credits.h"

#define FONT_SIZE 15

void retroDebug(const char *fmt, ...);

class RetroDream : public c2d::RoundedRectangleShape {

public:

    explicit RetroDream(c2d::Renderer *renderer, const c2d::Vector2f &size, float outlineThickness);

    c2d::Renderer *getRender() {
        return render;
    }

    Header *getHeader() {
        return header;
    }

    Preview *getPreview() {
        return preview;
    }

    Filer *getFiler() {
        return filer;
    }

    FileMenu *getFileMenu() {
        return fileMenu;
    }

    OptionMenu *getOptionMenu() {
        return optionMenu;
    }

    HelpBox *getHelpBox() {
        return helpBox;
    }

    Credits *getCredits() {
        return credits;
    }

    c2d::RectangleShape *getBlur() {
        return blurLayer;
    }

    void debugClockStart(const char *msg);

    void debugClockEnd(const char *msg);

    RetroConfig *getConfig();

    void showStatus(const std::string &title, const std::string &msg,
                    const c2d::Color &color = COL_RED);

    bool quit = false;

private:

    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    c2d::Renderer *render = nullptr;
    c2d::C2DClock timer;
    Filer *filer = nullptr;
    Filer *filerLeft = nullptr;
    Header *header = nullptr;
    Preview *preview = nullptr;
    OptionMenu *optionMenu = nullptr;
    FileMenu *fileMenu = nullptr;
    //StatusBox *statusBox = nullptr;
    HelpBox *helpBox = nullptr;
    Credits *credits = nullptr;
    c2d::RectangleShape *blurLayer = nullptr;
    int inputDelay;
    unsigned int oldKeys = 0;
    c2d::C2DClock debugClock;
};

#endif //RETRODREAM_MAIN_H
