//
// Created by cpasjuste on 09/02/2021.
//

#ifndef RETRODREAM_ROUNDEDTEXTURE_H
#define RETRODREAM_ROUNDEDTEXTURE_H

#include "cross2d/c2d.h"

class RoundedTexture : public c2d::C2DTexture {

public:
    explicit RoundedTexture(const std::string &path);

    explicit RoundedTexture(const c2d::Vector2f &size = c2d::Vector2f(0, 0), Format format = Format::RGBA8);

    std::size_t getPointCount() const override;

    c2d::Vector2f getPoint(std::size_t index) const override;

private:
    float myRadius;
    unsigned int myCornerPointCount;

};

#endif //RETRODREAM_ROUNDEDTEXTURE_H
