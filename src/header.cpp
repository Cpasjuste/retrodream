//
// Created by cpasjuste on 16/01/2020.
//

#include "header.h"
#include "colors.h"

using namespace c2d;

void Header::setString(const std::string &str) {
    text->setString(str);
}

Header::Header(const Vector2f &size, float radius, unsigned int cornerPointCount)
        : RoundedRectangleShape(size, radius, cornerPointCount) {

    text = new Text("RetroDream", (unsigned int) size.y - 10);
    text->setOrigin(Origin::Left);
    text->setPosition(10, (size.y / 2) - 1);
    text->setSizeMax(size.x - (float) text->getCharacterSize(), 0);
    add(text);
}

c2d::Text *Header::getText() {
    return text;
}
