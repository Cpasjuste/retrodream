//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "preview.h"

using namespace c2d;

Preview::Preview(const Vector2f &size, float radius, unsigned int cornerPointCount)
        : RoundedRectangleShape(size, radius, cornerPointCount) {
}

void Preview::load(const std::string &path) {

    if (texture != nullptr) {
        delete (texture);
        texture = nullptr;
    }

    if (path.empty()) {
        return;
    }

    texture = new C2DTexture(path);
    if (!texture->available) {
        delete (texture);
        texture = nullptr;
        return;
    }

    texture->setOrigin(Origin::Center);
    texture->setPosition(Vector2f(getSize().x / 2, getSize().y / 2));
    float tex_scaling = std::min(
            (getSize().x - 16) / texture->getTextureRect().width,
            (getSize().y - 16) / texture->getTextureRect().height);
    texture->setScale(tex_scaling, tex_scaling);
    add(texture);
}
