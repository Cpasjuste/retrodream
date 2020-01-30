//
// Created by cpasjuste on 30/01/2020.
//

#ifndef RETRODREAM_CREDITS_H
#define RETRODREAM_CREDITS_H

class Credits : public c2d::RoundedRectangleShape {

public:

    explicit Credits(RetroDream *rd, const c2d::FloatRect &rect);

    bool onInput(c2d::Input::Player *players) override;

private:

    RetroDream *retroDream = nullptr;
    c2d::Text *title = nullptr;
    c2d::Text *text = nullptr;
    c2d::Text *cpasjuste = nullptr;
};

#endif //RETRODREAM_CREDITS_H
