//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_MAIN_H
#define RETRODREAM_MAIN_H

#include "flash/romflash.h"
#include "colors.h"
#include "cfg.h"
#include "filer.h"
#include "header.h"
#include "preview.h"
#include "previewvideo.h"
#include "menu.h"
#include "presetmenu.h"
#include "filemenu.h"
#include "regionfreemenu.h"
#include "optionmenu.h"
#include "statusbox.h"
#include "utility.h"
#include "helpbox.h"
#include "credits.h"
#include "progressbox.h"
#include "gdplay.h"

void retroDebug(const char *fmt, ...);

class RetroDream : public SkinRect {

public:

    explicit RetroDream(c2d::Renderer *renderer, Skin::CustomShape *shape);

    c2d::Renderer *getRender() {
        return render;
    }

    RetroIo *getIo() {
        return (RetroIo *) render->getIo();
    }

    Header *getHeader() {
        return header;
    }

    Preview *getPreviewImage() {
        return preview;
    }

    PreviewVideo *getPreviewVideo() {
        return previewVideo;
    }

    Filer *getFiler() {
        return filer;
    }

    Menu *getPresetMenu() {
        return presetMenu;
    }

    Menu *getFileMenu() {
        return fileMenu;
    }

    Menu *getSystemMenu() {
        return systemMenu;
    }

    Menu *getRegionFreeMenu() {
        return regionFreeMenu;
    }

    Menu *getOptionMenu() {
        return optionMenu;
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

    static RetroConfig *getConfig();

    static Skin *getSkin();

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
    PreviewVideo *previewVideo = nullptr;
    Menu *optionMenu = nullptr;
    Menu *presetMenu = nullptr;
    Menu *fileMenu = nullptr;
    Menu *systemMenu = nullptr;
    Menu *regionFreeMenu = nullptr;
    HelpBox *helpBox = nullptr;
    Credits *credits = nullptr;
    ProgressBox *progressBox = nullptr;
    StatusBox *statusBox = nullptr;
    c2d::MessageBox *messageBox = nullptr;
    c2d::Text *fpsText = nullptr;
    char fpsBuffer[4]{};
    bool showFps = false;
    int inputDelay;
    unsigned int oldKeys = 0;
    c2d::C2DClock gdCheckClock;
};

#endif //RETRODREAM_MAIN_H
