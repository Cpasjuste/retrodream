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

    enum TweenType {
        Alpha = 0,
        Scale = 1
    };

    class CustomShape {
    public:
        c2d::FloatRect rect;
        int corners_radius;
        c2d::Origin origin = c2d::Origin::TopLeft;
        c2d::Color color;
        c2d::Color outlineColor;
        float outlineSize = 0;
        c2d::Color colorFile;
        c2d::Color colorDir;
        TweenType tweenType = Alpha;
    };

    enum OptionId {
        FilerPath = 0,
        ScreenSize = 1,
        InputDelay = 2,
        PreviewImageDelay = 3,
        PreviewVideoDelay = 4,
        FilerShape = 20,
        PreviewImageShape = 30,
        PreviewVideoShape = 40,
        HelpShape = 50,
    };

    explicit RetroConfig(RetroIo *io);

    std::string get(const OptionId &id);

    void set(const OptionId &id, const std::string &value, bool save = true);

    int getInt(const OptionId &id);

    void setInt(const OptionId &id, int value, bool save = true);

    c2d::FloatRect getRect(const OptionId &id);

    void setRect(const OptionId &id, const c2d::FloatRect &rect, bool save = true);

    CustomShape getShape(int groupId);

private:
    RetroIo *io = nullptr;
};

#endif //RETRODREAM_CFG_H
