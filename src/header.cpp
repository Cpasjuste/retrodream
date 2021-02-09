//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "header.h"

using namespace c2d;

Header::Header(const c2d::FloatRect &rect) : Rectangle(rect) {

    left = new RectangleShape({PERCENT(rect.width, 92), rect.height});
    left->setCornersRadius(CORNER_RADIUS);
    left->setCornerPointCount(CORNER_POINTS);
    textLeft = new Text("", FONT_SIZE);
    textLeft->setOrigin(Origin::Left);
    textLeft->setPosition(PERCENT(left->getSize().x, 1), (rect.height / 2));
    textLeft->setSizeMax(left->getSize().x - FONT_SIZE - 10, 0);
    left->add(textLeft);
    add(left);

    right = new RectangleShape({PERCENT(rect.width, 7.5f), rect.height});
    right->setCornersRadius(CORNER_RADIUS);
    right->setCornerPointCount(CORNER_POINTS);
    right->setPosition(PERCENT(rect.width, 93), 0);
    textRight = new Text("GDI", FONT_SIZE);
    textRight->setOrigin(Origin::Center);
    textRight->setPosition(right->getSize().x / 2 + 1, right->getSize().y / 2);
    textRight->setSizeMax(right->getSize().x, 0);
    right->add(textRight);
    add(right);
}

c2d::Text *Header::getTextLeft() {
    return textLeft;
}

c2d::Text *Header::getTextRight() {
    return textRight;
}

void Header::setString(const std::string &l, const std::string &r) {
    textLeft->setString(l);
    textRight->setString(r);
}

void Header::setStringLeft(const std::string &l) {
    textLeft->setString(l);
}

void Header::setStringRight(const std::string &r) {
    textRight->setString(r);
}

void Header::setFillColor(const c2d::Color &color) {
    left->setFillColor(color);
    right->setFillColor(color);
}

void Header::setOutlineColor(const c2d::Color &color) {
    left->setOutlineColor(color);
    right->setOutlineColor(color);
}

void Header::setOutlineThickness(float thickness) {
    left->setOutlineThickness(thickness);
    right->setOutlineThickness(thickness);
}

void Header::setStringRightColor(const c2d::Color &color) {
    right->setFillColor(color);
}
