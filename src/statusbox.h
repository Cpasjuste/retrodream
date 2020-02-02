//
// Created by cpasjuste on 08/12/18.
//

#ifndef PPLAY_STATUS_BOX_H
#define PPLAY_STATUS_BOX_H

class StatusBox : public c2d::Rectangle {

public:

    explicit StatusBox(RetroDream *rd, const c2d::FloatRect &rect);

    void show(const std::string &title, const std::string &message,
              const c2d::Color &color = COL_RED, bool infinite = false, bool drawNow = false);

    void hide();

private:

    void onDraw(c2d::Transform &transform, bool draw = true) override;

    RetroDream *retroDream = nullptr;
    c2d::Texture *icon = nullptr;
    c2d::TweenRotation *iconTween = nullptr;
    c2d::Text *titleText = nullptr;
    c2d::Text *messageText = nullptr;
    c2d::C2DClock clock;
    bool infinite = false;
};

#endif //PPLAY_STATUS_BOX_H
