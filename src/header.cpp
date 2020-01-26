//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"

using namespace c2d;

void Header::setString(const std::string &str) {
    text->setString(str);
}

Header::Header(const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    setPosition(rect.left, rect.top);
    float charSize = FONT_SIZE;
    text = new Text("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789-.", (unsigned int) charSize);
    text->setOrigin(Origin::Left);
    text->setPosition(PERCENT(rect.width, 2), (rect.height / 2) - 1);
    text->setSizeMax(rect.width - charSize - 10, 0);
    add(text);
}

c2d::Text *Header::getText() {
    return text;
}
