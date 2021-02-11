//
// Created by cpasjuste on 10/02/2021.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "skinrect.h"

using namespace c2d;

SkinRect::SkinRect(Skin::CustomShape *shape) : RectangleShape(shape->rect) {
    RectangleShape::setFillColor(shape->color);
    RectangleShape::setOrigin(shape->origin);
    RectangleShape::setOutlineColor(shape->outlineColor);
    RectangleShape::setOutlineThickness(shape->outlineSize);
    RectangleShape::setCornersRadius((float) shape->corners_radius);
    RectangleShape::setCornerPointCount(CORNER_POINTS);
    if (shape->tweenType == Skin::TweenType::Alpha) {
        RectangleShape::add(new TweenAlpha(0, 255, 0.3f));
    } else {
        RectangleShape::add(new TweenScale({0, 0}, {1, 1}, 0.2f));
    }
}
