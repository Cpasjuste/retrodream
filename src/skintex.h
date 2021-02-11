//
// Created by cpasjuste on 10/02/2021.
//

#ifndef RETRODREAM_SKINTEX_H
#define RETRODREAM_SKINTEX_H

#include "cross2d/skeleton/texture.h"
#include "skin.h"

class SkinTex : public c2d::C2DTexture {

public:

    explicit SkinTex(Skin::CustomShape *shape, const std::string &path);
};

#endif //RETRODREAM_SKINTEX_H
