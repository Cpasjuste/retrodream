//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"

using namespace c2d;

Preview::Preview(RetroConfig::CustomShape *shape) : RectangleShape(shape->rect) {

    Shape::setOrigin(Origin::Center);
    Shape::setFillColor(shape->color);
    Shape::setOutlineColor(shape->outlineColor);
    Shape::setOutlineThickness(shape->outlineSize);
    setCornersRadius(CORNER_RADIUS);
    setCornerPointCount(CORNER_POINTS);
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

    texture = new C2DTexture(path);
    if (!texture || !texture->available) {
        delete (texture);
        texture = nullptr;
    } else {
        texture->setCornerPointCount(CORNER_POINTS);
        texture->setCornersRadius(CORNER_RADIUS);
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
