//
// Created by cpasjuste on 20/01/2020.
//

#ifndef RETRODREAM_CFG_H
#define RETRODREAM_CFG_H

#include "cross2d/skeleton/config.h"

class RetroConfig : public c2d::config::Config {

public:
    explicit RetroConfig(const std::string &path);
};

#endif //RETRODREAM_CFG_H
