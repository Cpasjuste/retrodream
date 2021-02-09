//
// Created by cpasjuste on 16/01/2020.
//

#include "preview.h"
#include "roundedtexture.h"

using namespace c2d;

Preview::Preview(RetroConfig::CustomShape *shape)
        : RoundedRectangleShape({shape->rect.width, shape->rect.height}, 8, 4) {

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

    setVisibility(Visibility::Hidden);
}

bool Preview::load(const std::string &path) {

    printf("Preview::load: %s\n", path.c_str());

    unload();

    if (texture != nullptr) {
        delete (texture);
        texture = nullptr;
    }

    texture = new RoundedTexture(path);
    if (!texture || !texture->available) {
        delete (texture);
        texture = nullptr;
    } else {
        texture->setOrigin(Origin::Center);
        texture->setPosition(Vector2f(getSize().x / 2, getSize().y / 2));
        texture->setSize(getSize());
        add(texture);
        setVisibility(Visibility::Visible, true);
    }

    loaded = true;
    return true;
}

void Preview::unload() {
    loaded = false;
    setVisibility(Visibility::Hidden, true);
}

bool Preview::isLoaded() const {
    return loaded;
}
