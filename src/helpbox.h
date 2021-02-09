//
// Created by cpasjuste on 24/01/2020.
//

#ifndef RETRODREAM_HELPBOX_H
#define RETRODREAM_HELPBOX_H

class HelpBox : public c2d::RectangleShape {

public:

    enum HelpButton {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        Start = 4
    };

    class HelpLine : public c2d::Rectangle {
    public:
        HelpLine(const c2d::FloatRect &rect, const c2d::IntRect &spriteRect, c2d::Texture *texture);

        c2d::Sprite *button = nullptr;
        c2d::Text *text = nullptr;
    };

    explicit HelpBox(RetroDream *retroDream, RetroConfig::CustomShape *shape);

    ~HelpBox() override;

    void setString(HelpButton button, const std::string &text);

private:

    c2d::Texture *texture = nullptr;
    HelpLine *lines[5];
};

#endif //RETRODREAM_HELPBOX_H
