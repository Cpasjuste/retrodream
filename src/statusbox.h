//
// Created by cpasjuste on 08/12/18.
//

#ifndef PPLAY_STATUS_BOX_H
#define PPLAY_STATUS_BOX_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"

class StatusBox : public c2d::Rectangle {

public:

    explicit StatusBox(RetroDream *rd, const c2d::FloatRect &rect);

    ~StatusBox() override;

    void show(const std::string &title, const std::string &message, bool infinite = false, bool drawNow = false);

    void hide();

private:

    virtual void onDraw(c2d::Transform &transform, bool draw = true) override;

    RetroDream *retroDream;
    c2d::Vector2f pos;
    c2d::Texture *icon;
    c2d::Text *titleText;
    c2d::Text *messageText;
    c2d::C2DClock clock;
    //SDL_mutex *mutex;
    bool infinite = false;
#ifdef __DREAMCAST__
    //mutex_t mutex;
#endif
};

#endif //PPLAY_STATUS_BOX_H
