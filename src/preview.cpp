//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "preview.h"

using namespace c2d;

Preview::Preview(const Vector2f &size)
        : RoundedRectangleShape(size, 10, 8) {

    textureHolder = new Rectangle({size.x / 2, size.y / 2,
                                   size.x - 16, size.y - 16});
    textureHolder->setOrigin(Origin::Center);
    textureHolder->add(new TweenScale({0.1, 0.1}, {1, 1}, 0.1f));
    textureHolder->setVisibility(Visibility::Hidden);
    add(textureHolder);
}

void Preview::load(const std::string &path) {

    if (path.empty()) {
        textureHolder->setVisibility(Visibility::Hidden, true);
        loaded = false;
        return;
    }

    if (texture != nullptr) {
        delete (texture);
        texture = nullptr;
    }

    // set loaded to true so we don't loop on non-existing preview
    loaded = true;

    texture = new C2DTexture(path);
    if (!texture->available) {
        delete (texture);
        texture = nullptr;
        return;
    }

    texture->setOrigin(Origin::Center);
    texture->setPosition(Vector2f(textureHolder->getSize().x / 2, textureHolder->getSize().y / 2));
    float tex_scaling = std::min(
            textureHolder->getSize().x / (float) texture->getTextureRect().width,
            textureHolder->getSize().y / (float) texture->getTextureRect().height);
    texture->setScale(tex_scaling, tex_scaling);
    textureHolder->add(texture);
    textureHolder->setVisibility(Visibility::Visible, true);
}

void Preview::unload() {
    load();
}

bool Preview::isLoaded() {
    return loaded;
}
