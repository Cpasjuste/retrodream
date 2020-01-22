//
// Created by cpasjuste on 16/01/2020.
//

#include "header.h"
#include "utility.h"

using namespace c2d;

void Header::setString(const std::string &str) {
    text->setString(str);
}

Header::Header(const c2d::FloatRect &rect, float radius, unsigned int cornerPointCount)
        : RoundedRectangleShape({rect.width, rect.height}, radius, cornerPointCount) {

    setPosition(rect.left, rect.top);
    float charSize = rect.height - 5;
    text = new Text("RetroDream", (unsigned int) charSize);
    text->setOrigin(Origin::Left);
    text->setPosition(PERCENT(rect.width, 2), (rect.height / 2) - 1);
    text->setSizeMax(rect.width - charSize - 10, 0);
    add(text);
}

c2d::Text *Header::getText() {
    return text;
}
