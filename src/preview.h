//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEW_H
#define RETRODREAM_PREVIEW_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"
#include "cfg.h"

class RetroDream;

class Preview : public c2d::RectangleShape {

public:

    explicit Preview(RetroConfig::CustomShape *shape);

    bool load(const std::string &path);

    void unload();

    bool isLoaded() const;

private:

    c2d::RectangleShape *outline = nullptr;
    c2d::Texture *texture = nullptr;
    bool loaded = false;
};

#endif //RETRODREAM_PREVIEW_H
