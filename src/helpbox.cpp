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

HelpBox::HelpBox(RetroDream *retroDream, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    setPosition(rect.left, rect.top);

    texture = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/buttons.png");

    lines[HelpButton::X] = new HelpLine(
            {2, 4, rect.width, 32},
            {0, 0, 32, 32}, texture);
    lines[HelpButton::X]->text->setString("FILE OPTIONS");
    add(lines[HelpButton::X]);

    lines[HelpButton::A] = new HelpLine(
            {2, 36, rect.width, 32},
            {32, 32, 32, 32}, texture);
    add(lines[HelpButton::A]);

    lines[HelpButton::Y] = new HelpLine(
            {rect.width / 2 - 4, 4, rect.width, 32},
            {32, 0, 32, 32}, texture);
    add(lines[HelpButton::Y]);

    lines[HelpButton::B] = new HelpLine(
            {rect.width / 2 - 4, 36, rect.width, 32},
            {0, 32, 32, 32}, texture);
    lines[HelpButton::B]->text->setString("BACK");
    add(lines[HelpButton::B]);

    lines[HelpButton::Start] = new HelpLine(
            {2, 68, rect.width, 32},
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
