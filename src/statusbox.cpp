//
// Created by cpasjuste on 08/12/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "statusbox.h"

using namespace c2d;

StatusBox::StatusBox(const c2d::FloatRect &rect) : Rectangle(rect) {

    titleText = new Text("TITLE TEXT", FONT_SIZE);
    titleText->setFillColor(COL_RED);
    titleText->setOutlineColor(Color::Black);
    titleText->setOutlineThickness(2);
    titleText->setSizeMax(rect.width - FONT_SIZE, FONT_SIZE + 4);
    add(titleText);

    messageText = new Text("MESSAGE TEXT", FONT_SIZE);
    messageText->setPosition(0, FONT_SIZE + 6);
    messageText->setScale(0.9f, 0.9f);
    messageText->setFillColor(COL_RED);
    messageText->setOutlineColor(Color::Black);
    messageText->setOutlineThickness(2);
    messageText->setSizeMax(rect.width - FONT_SIZE, FONT_SIZE + 4);
    add(messageText);

    setVisibility(Visibility::Hidden);
}

void StatusBox::show(const std::string &title, const std::string &msg, const c2d::Color &color) {

    if (titleText == nullptr || messageText == nullptr) {
        return;
    }

    titleText->setString(Utility::toUpper(title));
    messageText->setString(Utility::toUpper(msg));

    Color c = titleText->getFillColor();
    if (c != color) {
        titleText->setFillColor(color);
        messageText->setFillColor(color);
    }

    if (!isVisible()) {
        setVisibility(Visibility::Visible);
    }

    clock.restart();
}

void StatusBox::onUpdate() {

    if (isVisible() && clock.getElapsedTime().asSeconds() > 2) {
        setVisibility(Visibility::Hidden);
    }

    Rectangle::onUpdate();
}
