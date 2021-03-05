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
    Rectangle::add(button);

    text = new Text("UNUSED", FONT_SIZE);
    text->setOrigin(Origin::Left);
    text->setPosition(button->getPosition().x + button->getSize().x + 4, (rect.height / 2) - 1);
    //text->setOutlineThickness(2);
    text->setOutlineColor(Color::Black);
    Rectangle::add(text);
}

HelpBox::HelpBox(RetroDream *retroDream, Skin::CustomShape *shape) : SkinRect(shape) {

    float left = 6;
    float width = SkinRect::getSize().x - 16;

    texture = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/buttons.png");
    lines[HelpButton::X] = new HelpLine(
            {left, 4, width, 32},
            {0, 0, 32, 32}, texture);
    lines[HelpButton::X]->text->setString("FILE OPTIONS");
    SkinRect::add(lines[HelpButton::X]);

    lines[HelpButton::A] = new HelpLine(
            {left, 36, width, 32},
            {32, 32, 32, 32}, texture);
    SkinRect::add(lines[HelpButton::A]);

    lines[HelpButton::Y] = new HelpLine(
            {left, 68, width, 32},
            {32, 0, 32, 32}, texture);
    SkinRect::add(lines[HelpButton::Y]);

    lines[HelpButton::B] = new HelpLine(
            {left, 100, width, 32},
            {0, 32, 32, 32}, texture);
    lines[HelpButton::B]->text->setString("BACK");
    SkinRect::add(lines[HelpButton::B]);

    lines[HelpButton::Start] = new HelpLine(
            {left, 132, width, 32},
            {64, 0, 32, 32}, texture);
    lines[HelpButton::Start]->text->setString("MAIN MENU");
    SkinRect::add(lines[HelpButton::Start]);
}

void HelpBox::setString(HelpBox::HelpButton button, const std::string &text) {
    lines[button]->text->setString(text);
}

HelpBox::~HelpBox() {
    if (texture != nullptr) {
        delete (texture);
    }
}
