//
// Created by cpasjuste on 24/01/2020.
//

#include <cross2d/c2d.h>
#include "main.h"
#include "helpbox.h"

using namespace c2d;

HelpBox::HelpLine::HelpLine(const c2d::FloatRect &rect, const c2d::IntRect &spriteRect, c2d::Texture *texture)
        : Rectangle(rect) {

    button = new Sprite(texture, spriteRect);
    button->setOrigin(Origin::Left);
    button->setPosition(0, rect.height / 2);
    button->setScale(0.9f, 0.9f);
    add(button);

    text = new Text("UNUSED", FONT_SIZE);
    text->setOrigin(Origin::Left);
    text->setPosition(button->getPosition().x + button->getSize().x, (rect.height / 2) - 1);
    text->setOutlineThickness(2);
    text->setOutlineColor(Color::Black);
    add(text);
}

HelpBox::HelpBox(RetroDream *retroDream, RetroConfig::CustomShape *shape)
        : RoundedRectangleShape({288, 103}, 8, 4) {

    setOrigin(Origin::Center);
    setPosition(shape->rect.left, shape->rect.top);
    setFillColor(shape->color);
    setOutlineColor(shape->outlineColor);
    setOutlineThickness(shape->outlineSize);
    if (shape->tweenType == RetroConfig::TweenType::Alpha) {
        add(new TweenAlpha(0, 255, 0.5f));
    } else {
        add(new TweenScale({0, 0}, {1, 1}, 0.3f));
    }

    float width = getSize().x;

    texture = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/buttons.png");

    lines[HelpButton::X] = new HelpLine(
            {2, 4, width, 32},
            {0, 0, 32, 32}, texture);
    lines[HelpButton::X]->text->setString("FILE OPTIONS");
    add(lines[HelpButton::X]);

    lines[HelpButton::A] = new HelpLine(
            {2, 36, width, 32},
            {32, 32, 32, 32}, texture);
    add(lines[HelpButton::A]);

    lines[HelpButton::Y] = new HelpLine(
            {width / 2 - 4, 4, width, 32},
            {32, 0, 32, 32}, texture);
    add(lines[HelpButton::Y]);

    lines[HelpButton::B] = new HelpLine(
            {width / 2 - 4, 36, width, 32},
            {0, 32, 32, 32}, texture);
    lines[HelpButton::B]->text->setString("BACK");
    add(lines[HelpButton::B]);

    lines[HelpButton::Start] = new HelpLine(
            {2, 68, width, 32},
            {64, 0, 32, 32}, texture);
    lines[HelpButton::Start]->text->setString("MAIN MENU");
    add(lines[HelpButton::Start]);
}

void HelpBox::setString(HelpBox::HelpButton button, const std::string &text) {
    lines[button]->text->setString(text);
}

HelpBox::~HelpBox() {
    if (texture != nullptr) {
        delete (texture);
    }
}
