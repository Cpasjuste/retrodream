//
// Created by cpasjuste on 10/02/2021.
//

#ifndef RETRODREAM_SKINRECT_H
#define RETRODREAM_SKINRECT_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"
#include "skin.h"

class SkinRect : public c2d::RectangleShape {

public:

    explicit SkinRect(Skin::CustomShape *shape, bool tween = true);
};

#endif //RETRODREAM_SKINRECT_H
