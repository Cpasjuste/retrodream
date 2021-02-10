//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"

using namespace c2d;

Preview::Preview(RetroConfig::CustomShape *shape) : SkinRect(shape) {
    RectangleShape::setFillColor(Color::Transparent);
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
    if (!texture->available) {
        delete (texture);
        texture = nullptr;
    } else {
        texture->setCornerPointCount(CORNER_POINTS);
        texture->setCornersRadius(getCornersRadius());
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
