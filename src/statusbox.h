//
// Created by cpasjuste on 08/12/18.
//

#ifndef STATUS_BOX_H
#define STATUS_BOX_H

class StatusBox : public c2d::Rectangle {

public:

    explicit StatusBox(const c2d::FloatRect &rect);

    void show(const std::string &title, const std::string &msg, const c2d::Color &color = COL_RED);

private:

    void onUpdate() override;

    c2d::Text *titleText = nullptr;
    c2d::Text *messageText = nullptr;
    c2d::C2DClock clock;
    float timer = 5.0f;
};

#endif //STATUS_BOX_H
