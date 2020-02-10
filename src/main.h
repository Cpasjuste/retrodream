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
#include "presetmenu.h"
#include "filemenu.h"
#include "statusbox.h"
#include "utility.h"
#include "helpbox.h"
#include "credits.h"
#include "progressbox.h"

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

    PresetMenu *getPresetMenu() {
        return presetMenu;
    }

    OptionMenu *getOptionMenu() {
        return optionMenu;
    }

    FileMenu *getFileMenu() {
        return fileMenu;
    }

    HelpBox *getHelpBox() {
        return helpBox;
    }

    Credits *getCredits() {
        return credits;
    }

    ProgressBox *getProgressBox() {
        return progressBox;
    }

    c2d::MessageBox *getMessageBox() {
        return messageBox;
    }

    void debugClockStart(const char *msg);

    void debugClockEnd(const char *msg);

    RetroConfig *getConfig();

    void showStatus(const std::string &title, const std::string &msg, const c2d::Color &color = COL_RED);

    bool quit = false;

private:

    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    c2d::Renderer *render = nullptr;
    c2d::C2DClock timer;
    Filer *filer = nullptr;
    Header *header = nullptr;
    Preview *preview = nullptr;
    OptionMenu *optionMenu = nullptr;
    PresetMenu *presetMenu = nullptr;
    FileMenu *fileMenu = nullptr;
    HelpBox *helpBox = nullptr;
    Credits *credits = nullptr;
    ProgressBox *progressBox = nullptr;
    StatusBox *statusBox = nullptr;
    c2d::MessageBox *messageBox = nullptr;
    int inputDelay;
    unsigned int oldKeys = 0;
    c2d::C2DClock debugClock;
};

#endif //RETRODREAM_MAIN_H
