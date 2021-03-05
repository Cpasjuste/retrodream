//
// Created by cpasjuste on 08/12/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "statusbox.h"

using namespace c2d;

StatusBox::StatusBox(const c2d::FloatRect &rect) : RectangleShape(rect) {

    Color color = COL_RED;
    color.a = 150;
    RectangleShape::setFillColor(color);
    RectangleShape::setOutlineColor(COL_RED);
    RectangleShape::setOutlineThickness(3);
    RectangleShape::setCornersRadius(8);
    RectangleShape::setCornerPointCount(CORNER_POINTS);

    titleText = new Text("TITLE TEXT", FONT_SIZE);
    titleText->setPosition(5, 7);
    titleText->setFillColor(COL_RED);
    titleText->setOutlineColor(Color::Black);
    titleText->setOutlineThickness(2);
    titleText->setSizeMax(rect.width - FONT_SIZE, FONT_SIZE + 4);
    RectangleShape::add(titleText);

    messageText = new Text("MESSAGE TEXT", FONT_SIZE);
    messageText->setPosition(5, FONT_SIZE + 14);
    messageText->setScale(0.8f, 0.8f);
    messageText->setFillColor(COL_RED);
    messageText->setOutlineColor(Color::Black);
    messageText->setOutlineThickness(2);
    messageText->setSizeMax(rect.width - FONT_SIZE, FONT_SIZE + 4);
    RectangleShape::add(messageText);

    Vector2f pos = RectangleShape::getPosition();
    RectangleShape::add(new TweenPosition({pos.x, pos.y + rect.height},
                                          RectangleShape::getPosition(), 0.2f));

    RectangleShape::setVisibility(Visibility::Hidden);
}

void StatusBox::show(const std::string &title, const std::string &msg, const c2d::Color &color) {

    if (titleText == nullptr || messageText == nullptr) {
        return;
    }

    titleText->setString(Utility::toUpper(title));
    messageText->setString(Utility::toUpper(msg));

    float titleWidth = titleText->getLocalBounds().width;
    float messageWidth = messageText->getLocalBounds().width;
    float width = titleWidth > messageWidth ? titleWidth : messageWidth;
    setSize(width, getSize().y);

    Color c = titleText->getFillColor();
    if (c != color) {
        titleText->setFillColor(color);
        messageText->setFillColor(color);
        RectangleShape::setOutlineColor(color);
        c = color;
        c.a = 150;
        RectangleShape::setFillColor(c);
    }

    if (!isVisible()) {
        setVisibility(Visibility::Visible, true);
    }

    clock.restart();
}

void StatusBox::onUpdate() {

    if (isVisible() && clock.getElapsedTime().asSeconds() > timer) {
        setVisibility(Visibility::Hidden, true);
    }

    RectangleShape::onUpdate();
}
