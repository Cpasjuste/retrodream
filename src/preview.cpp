//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"

using namespace c2d;

Preview::Preview(RetroConfig::CustomShape *shape) : RectangleShape(shape->rect) {

    RectangleShape::setOrigin(Origin::Center);
    RectangleShape::setFillColor(Color::Transparent);
    RectangleShape::setOutlineColor(Color::Black);
    RectangleShape::setOutlineThickness(shape->outlineSize + 2);
    RectangleShape::setCornersRadius(CORNER_RADIUS);
    RectangleShape::setCornerPointCount(CORNER_POINTS);
    if (shape->tweenType == RetroConfig::TweenType::Alpha) {
        RectangleShape::add(new TweenAlpha(0, 255, 0.3f));
    } else {
        RectangleShape::add(new TweenScale({0, 0}, {1, 1}, 0.2f));
    }

    outline = new RectangleShape({shape->rect.width, shape->rect.height});
    outline->setFillColor(Color::Transparent);
    outline->setOutlineColor(shape->outlineColor);
    outline->setOutlineThickness(shape->outlineSize);
    outline->setCornersRadius(CORNER_RADIUS);
    outline->setCornerPointCount(CORNER_POINTS);
    RectangleShape::add(outline);

    RectangleShape::setVisibility(Visibility::Hidden);
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
        outline->add(texture);
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
