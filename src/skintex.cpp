//
// Created by cpasjuste on 10/02/2021.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "skintex.h"

using namespace c2d;

SkinTex::SkinTex(Skin::CustomShape *shape, const std::string &path) : C2DTexture(path) {
    C2DTexture::setFillColor(shape->color);
    C2DTexture::setOrigin(shape->origin);
    C2DTexture::setOutlineColor(shape->outlineColor);
    C2DTexture::setOutlineThickness(shape->outlineSize);
    C2DTexture::setCornersRadius((float) shape->corners_radius);
    C2DTexture::setCornerPointCount(CORNER_POINTS);
    if (shape->tweenType == Skin::TweenType::Alpha) {
        C2DTexture::add(new TweenAlpha(0, 255, 0.3f));
    } else {
        C2DTexture::add(new TweenScale({0, 0}, {1, 1}, 0.2f));
    }
}
