//
// Created by cpasjuste on 20/01/2020.
//

#ifndef RETRODREAM_CFG_H
#define RETRODREAM_CFG_H

#include "cross2d/skeleton/io.h"
#include "cross2d/skeleton/config.h"

class RetroIo;

class RetroConfig : public c2d::config::Config {

public:
    explicit RetroConfig(c2d::Io *io);

    std::string getDataPath();

    std::string getLastPath();

    void setLastPath(const std::string &path);

private:
    RetroIo *io = nullptr;
};

#endif //RETRODREAM_CFG_H
