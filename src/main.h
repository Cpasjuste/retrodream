//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_MAIN_H
#define RETRODREAM_MAIN_H

#include "filer.h"
#include "header.h"
#include "preview.h"

class RetroDream : public c2d::RectangleShape {

public:

    explicit RetroDream(c2d::Renderer *renderer, const c2d::Vector2f &size);

    ~RetroDream() override;

    c2d::Renderer *getRender();

    Header *getHeader();

    Preview *getPreview();

    bool quit = false;

private:

    bool onInput(c2d::Input::Player *players) override;

    void onDraw(c2d::Transform &transform, bool draw = true) override;

    c2d::Renderer *renderer = nullptr;
    c2d::C2DClock timer;
    Filer *filer = nullptr;
    Filer *filerLeft = nullptr;
    Filer *filerRight = nullptr;
    Header *header = nullptr;
    Preview *preview = nullptr;
};

#define INPUT_DELAY 200

#endif //RETRODREAM_MAIN_H
