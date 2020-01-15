//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_MAIN_H
#define RETRODREAM_MAIN_H

#include "filer.h"

#define INPUT_DELAY 200

#define COL_GREEN       Color(153, 255, 51)
#define COL_YELLOW      Color(255, 225, 51)
#define COL_ORANGE      Color(255, 153, 51)
#define COL_RED         Color(255, 51, 51)
#define COL_GRAY        Color(40, 40, 40)
#define COL_GRAY_LIGHT  Color(70, 70, 70)
#define COL_BLUE        Color(23,86,155)
#define COL_BLUE_LIGHT  Color(29,108,194)

class RetroDream : public c2d::C2DRenderer {

public:

    explicit RetroDream(const c2d::Vector2f &size);

    ~RetroDream() override;

    bool quit = false;

private:

    bool onInput(c2d::Input::Player *players) override;

    void onDraw(c2d::Transform &transform, bool draw = true) override;

    Filer *filer = nullptr;
    c2d::C2DClock timer;
};

#endif //RETRODREAM_MAIN_H
