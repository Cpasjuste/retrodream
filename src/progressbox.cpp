//
// Created by cpasjuste on 04/02/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "progressbox.h"


using namespace c2d;

ProgressBox::ProgressBox(RetroDream *rd, const c2d::Color &fill, const c2d::Color &out, const c2d::Color &textFill)
        : RectangleShape(Vector2f(rd->getSize().x / 1.6f, rd->getSize().y / 1.6f)) {

    float w = rd->getSize().x / 1.6f;
    float h = rd->getSize().y / 1.6f;
    float margin = 16;

    setOrigin(Origin::Center);
    setPosition(rd->getSize().x / 2, rd->getSize().y / 2);
    setFillColor(fill);
    setOutlineColor(out);
    setOutlineThickness(5);

    title = new Text("TITLE", FONT_SIZE);
    title->setPosition(margin, margin);
    title->setSizeMax(w - (margin * 2), 0);
    title->setFillColor(textFill);
    title->setOutlineThickness(2);
    title->setOutlineColor(Color::Black);
    add(title);

    message = new Text("MESSAGE", FONT_SIZE);
    message->setPosition(margin, 64);
    message->setSizeMax(w - (margin * 2), h - 192);
    message->setLineSpacingModifier(6);
    message->setOverflow(Text::Overflow::NewLine);
    message->setFillColor(textFill);
    message->setOutlineThickness(2);
    message->setOutlineColor(Color::Black);
    add(message);

    progress_bg = new RectangleShape(
            FloatRect(margin, h - margin - (h / 6),
                      w - (margin * 2), h / 6));
    Color col = fill;
    col.r = (uint8_t) std::max(0, col.r - 40);
    col.g = (uint8_t) std::max(0, col.g - 40);
    col.b = (uint8_t) std::max(0, col.b - 40);
    progress_bg->setFillColor(col);
    progress_bg->setOutlineColor(out);
    progress_bg->setOutlineThickness(2);
    add(progress_bg);

    progress_fg = new RectangleShape(
            FloatRect(progress_bg->getPosition().x + 1, progress_bg->getPosition().y + 1,
                      2, progress_bg->getSize().y - 2));
    progress_fg->setFillColor(out);
    add(progress_fg);

    progress_message = new Text("PROGRESS MESSAGE", FONT_SIZE);
    progress_message->setPosition(margin, progress_bg->getPosition().y - FONT_SIZE - 16);
    progress_message->setSizeMax(w - (margin * 3), FONT_SIZE + 4);
    progress_message->setFillColor(textFill);
    progress_message->setOutlineThickness(2);
    progress_message->setOutlineColor(Color::Black);
    add(progress_message);

    setVisibility(Visibility::Hidden);
}

void ProgressBox::setTitle(const std::string &text) {

    title->setString(text);
}

void ProgressBox::setMessage(const std::string &text) {

    message->setString(text);
}

void ProgressBox::setProgress(const std::string &text, float progress) {

    float width = progress * (progress_bg->getSize().x - 2);
    progress_fg->setSize(
            std::min(width, progress_bg->getSize().x - 2),
            progress_fg->getSize().y);
    progress_message->setString(text);
}

c2d::Text *ProgressBox::getTitle() {
    return title;
}
