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

HelpBox::HelpBox(RetroDream *retroDream, Skin::CustomShape *shape) : SkinRect(shape) {

    // HelpBox use scaling for size (original size: 288 x 103)
    SkinRect::setSize(288, 103);
    float width = 288;

    texture = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/buttons.png");

    lines[HelpButton::X] = new HelpLine(
            {2, 4, width, 32},
            {0, 0, 32, 32}, texture);
    lines[HelpButton::X]->text->setString("FILE OPTIONS");
    SkinRect::add(lines[HelpButton::X]);

    lines[HelpButton::A] = new HelpLine(
            {2, 36, width, 32},
            {32, 32, 32, 32}, texture);
    SkinRect::add(lines[HelpButton::A]);

    lines[HelpButton::Y] = new HelpLine(
            {width / 2 - 4, 4, width, 32},
            {32, 0, 32, 32}, texture);
    SkinRect::add(lines[HelpButton::Y]);

    lines[HelpButton::B] = new HelpLine(
            {width / 2 - 4, 36, width, 32},
            {0, 32, 32, 32}, texture);
    lines[HelpButton::B]->text->setString("BACK");
    SkinRect::add(lines[HelpButton::B]);

    lines[HelpButton::Start] = new HelpLine(
            {2, 68, width, 32},
            {64, 0, 32, 32}, texture);
    lines[HelpButton::Start]->text->setString("MAIN MENU");
    SkinRect::add(lines[HelpButton::Start]);

    // scaling (original size: 288 x 103)
    SkinRect::setScale(shape->rect.width / 288, shape->rect.height / 103);
}

void HelpBox::setString(HelpBox::HelpButton button, const std::string &text) {
    lines[button]->text->setString(text);
}

HelpBox::~HelpBox() {
    if (texture != nullptr) {
        delete (texture);
    }
}
