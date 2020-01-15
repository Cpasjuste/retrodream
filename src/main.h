//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_MAIN_H
#define RETRODREAM_MAIN_H

#include "filer.h"

class RetroDream : public c2d::RectangleShape {

public:

    explicit RetroDream(c2d::Renderer *renderer, const c2d::Vector2f &size);

    ~RetroDream() override;

    c2d::Renderer *getRender();

    bool quit = false;

private:

    bool onInput(c2d::Input::Player *players) override;

    void onDraw(c2d::Transform &transform, bool draw = true) override;

    c2d::Renderer *renderer = nullptr;
    c2d::C2DClock timer;
    Filer *filer = nullptr;
    Filer *filerLeft = nullptr;
    Filer *filerRight = nullptr;
};

#define INPUT_DELAY 200

#define COL_GREEN       Color(153, 255, 51)
#define COL_YELLOW      Color(255, 225, 51)
#define COL_ORANGE      Color(255, 153, 51)
#define COL_RED         Color(255, 51, 51)
#define COL_GRAY        Color(40, 40, 40)
#define COL_GRAY_LIGHT  Color(70, 70, 70)
#define COL_BLUE        Color(97,190,236)
#define COL_BLUE_LIGHT  Color(178,226,249)
#define COL_BLUE_DARK   Color(49,121,159)
#define COL_BLUE_GRAY   Color(204,228,240)

#endif //RETRODREAM_MAIN_H
