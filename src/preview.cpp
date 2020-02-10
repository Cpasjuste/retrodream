//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "preview.h"

using namespace c2d;

Preview::Preview(const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.1f));
    setVisibility(Visibility::Hidden);
}

bool Preview::load(const std::string &path) {

    if (path.empty()) {
        if (isVisible()) {
            setVisibility(Visibility::Hidden, true);
        }
        loaded = false;
        return true;
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
        return false;
    }

    texture->setOrigin(Origin::Left);
    texture->setPosition(Vector2f(8, getSize().y / 2));
    float tex_scaling = std::min(
            getSize().x / ((float) texture->getTextureRect().width + 16),
            getSize().y / ((float) texture->getTextureRect().height + 16));
    texture->setScale(tex_scaling, tex_scaling);
    add(texture);
    setVisibility(Visibility::Visible, true);

    return true;
}

void Preview::unload() {
    if (loaded) {
        load();
    }
}

bool Preview::isLoaded() {
    return loaded;
}
