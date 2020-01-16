//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEW_H
#define RETRODREAM_PREVIEW_H

#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"

class Preview : public c2d::RoundedRectangleShape {

public:

    Preview(const c2d::Vector2f &size, float radius, unsigned int cornerPointCount);

    void load(const std::string &path = "");

private:

    c2d::Texture *texture = nullptr;

};

#endif //RETRODREAM_PREVIEW_H
