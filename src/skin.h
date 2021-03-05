//
// Created by cpasjuste on 11/02/2021.
//

#ifndef RETRODREAM_SKIN_H
#define RETRODREAM_SKIN_H

#include "cross2d/skeleton/config.h"
#include "retroio.h"

#define FONT_SIZE 18
#define CORNER_RADIUS 8
#define CORNER_POINTS 4

class Skin : public c2d::config::Config {

public:
    enum TweenType {
        Alpha = 0,
        Scale = 1
    };

    enum Id {
        BackgroundShape,
        LogoShape,
        FilerShape,
        FilerHighlightShape,
        FilerPathShape,
        FilerBarText,
        FilerTextSpacing,
        FilerFileText,
        FilerDirText,
        PreviewImageShape,
        PreviewVideoShape,
        HelpShape,
        MenuShape,
        MenuLeftText,
        MenuRightText,
        MenuHighlightShape
    };

    class CustomShape {
    public:
        c2d::FloatRect rect;
        int corners_radius = 0;
        c2d::Origin origin = c2d::Origin::TopLeft;
        c2d::Color color = c2d::Color::Transparent;
        c2d::Color outlineColor = c2d::Color::Transparent;
        float outlineSize = 0;
        TweenType tweenType = Scale;
    };

    class CustomColor {
    public:
        c2d::Color color = c2d::Color::Transparent;
        c2d::Color outlineColor = c2d::Color::Transparent;
        float outlineSize = 0;
    };

    class CustomText {
    public:
        c2d::Color color = c2d::Color::White;
        int size = 18;
    };

    explicit Skin(RetroIo *retroIo);

    CustomShape getShape(int groupId);

    CustomColor getColor(int groupId);

    CustomText getText(int groupId);

private:
    RetroIo *io = nullptr;

    c2d::config::Group addShape(const std::string &name, int id,
                                const c2d::FloatRect &rect, int corners_radius, const c2d::Origin &origin,
                                const c2d::Color &color, const c2d::Color &outlineColor, int outlineSize,
                                int tween_type);

    c2d::config::Group addColor(const std::string &name, int id,
                                const c2d::Color &color, const c2d::Color &outlineColor, int outlineSize);

    c2d::config::Group addText(const std::string &name, int id,
                               const c2d::Color &color, int size);
};

#endif //RETRODREAM_SKIN_H
