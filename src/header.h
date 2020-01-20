//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_HEADER_H
#define RETRODREAM_HEADER_H

#include "cross2d/skeleton/sfml/Text.hpp"
#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"

class Header : public c2d::RoundedRectangleShape {

public:

    Header(const c2d::Vector2f &size, float radius, unsigned int cornerPointCount);

    c2d::Text *getText();

    void setString(const std::string &str);

private:

    c2d::Text *text = nullptr;
    std::vector<std::string> messages;
};

#endif //RETRODREAM_HEADER_H
