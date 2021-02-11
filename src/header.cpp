//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "header.h"

using namespace c2d;

Header::Header(Skin::CustomShape *shape) : Rectangle(shape->rect) {

    // left box
    shape->rect.width = PERCENT(Rectangle::getSize().x, 92);
    left = new SkinRect(shape);
    left->setPosition(0, 0);
    Rectangle::add(left);
    // left box text
    Skin::CustomColor colors = RetroDream::getSkin()->getColor(Skin::Id::FilerBarText);
    textLeft = new Text("", FONT_SIZE);
    textLeft->setFillColor(colors.color);
    textLeft->setOutlineColor(colors.outlineColor);
    textLeft->setOutlineThickness(colors.outlineSize);
    textLeft->setOrigin(Origin::Left);
    textLeft->setPosition(PERCENT(left->getSize().x, 1), (Rectangle::getSize().y / 2));
    textLeft->setSizeMax(left->getSize().x - FONT_SIZE - 10, 0);
    left->add(textLeft);

    // right box
    shape->rect.width = PERCENT(Rectangle::getSize().x, 7.5f);
    right = new SkinRect(shape);
    right->setPosition(PERCENT(Rectangle::getSize().x, 93), 0);
    Rectangle::add(right);
    // right box text
    textRight = new Text("GDI", FONT_SIZE);
    textRight->setFillColor(colors.color);
    textRight->setOutlineColor(colors.outlineColor);
    textRight->setOutlineThickness(colors.outlineSize);
    textRight->setOrigin(Origin::Center);
    textRight->setPosition(right->getSize().x / 2, Rectangle::getSize().y / 2);
    textRight->setSizeMax(right->getSize().x, 0);
    right->add(textRight);
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
