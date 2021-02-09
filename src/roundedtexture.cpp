//
// Created by cpasjuste on 09/02/2021.
//

#include "cross2d/c2d.h"
#include "roundedtexture.h"

using namespace c2d;

RoundedTexture::RoundedTexture(const std::string &path) : C2DTexture(path) {
    myRadius = 8;
    myCornerPointCount = 4;
}

RoundedTexture::RoundedTexture(const Vector2f &size, Texture::Format format) : C2DTexture(size, format) {
    myRadius = 8;
    myCornerPointCount = 4;
}

std::size_t RoundedTexture::getPointCount() const {
    return myCornerPointCount * 4;
}

c2d::Vector2f RoundedTexture::getPoint(std::size_t index) const {
    if (index >= myCornerPointCount * 4) {
        return {0, 0};
    }

    float deltaAngle = 90.0f / (myCornerPointCount - 1);
    Vector2f center;
    unsigned int centerIndex = index / myCornerPointCount;
    static const float pi = 3.141592654f;
    Vector2f mySize = getSize();

    switch (centerIndex) {
        case 0:
            center.x = mySize.x - myRadius;
            center.y = myRadius;
            break;
        case 1:
            center.x = myRadius;
            center.y = myRadius;
            break;
        case 2:
            center.x = myRadius;
            center.y = mySize.y - myRadius;
            break;
        case 3:
            center.x = mySize.x - myRadius;
            center.y = mySize.y - myRadius;
            break;
    }

    return {myRadius * cos(deltaAngle * (index - centerIndex) * pi / 180) + center.x,
            -myRadius * sin(deltaAngle * (index - centerIndex) * pi / 180) + center.y};
}
