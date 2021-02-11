//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_HEADER_H
#define RETRODREAM_HEADER_H

#include "cross2d/skeleton/sfml/Text.hpp"
#include "cross2d/skeleton/sfml/RectangleShape.hpp"

class Header : public c2d::Rectangle {

public:

    explicit Header(Skin::CustomShape *shape);

    c2d::Text *getTextLeft();

    c2d::Text *getTextRight();

    void setString(const std::string &left, const std::string &right);

    void setStringLeft(const std::string &left);

    void setStringRight(const std::string &right);

    void setStringRightColor(const c2d::Color &color);

    void setFillColor(const c2d::Color &color) override;

    void setOutlineColor(const c2d::Color &color) override;

    void setOutlineThickness(float thickness) override;

private:

    c2d::Text *textLeft = nullptr;
    c2d::Text *textRight = nullptr;
    c2d::RectangleShape *left = nullptr;
    c2d::RectangleShape *right = nullptr;
};

#endif //RETRODREAM_HEADER_H
