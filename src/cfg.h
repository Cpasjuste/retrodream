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

    enum OptionId {
        FilerPath = 0,
        ScreenSize = 1,
        InputDelay = 2
    };

    explicit RetroConfig(RetroIo *io);

    std::string get(const OptionId &id);

    void set(const OptionId &id, const std::string &value, bool save = true);

    int getInt(const OptionId &id);

    void setInt(const OptionId &id, int value, bool save = true);

    c2d::FloatRect getRect(const OptionId &id);

    void setRect(const OptionId &id, const c2d::FloatRect &rect, bool save = true);

private:
    RetroIo *io = nullptr;
};

#endif //RETRODREAM_CFG_H
