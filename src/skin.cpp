//
// Created by cpasjuste on 11/02/2021.
//

#include "cross2d/c2d.h"
#include "skin.h"

using namespace c2d;

Skin::Skin(RetroIo *retroIo) : Config("SkinConfig", retroIo->getSkinPath()) {

    io = retroIo;

    // background
    Group background = addShape("background", BackgroundShape,
                                FloatRect{6, 6, 628, 468},
                                8, Origin::TopLeft,
                                Color{97, 190, 236},
                                Color{49, 121, 159},
                                12, 0);
    addGroup(background);

    // filer_bar
    Group filer_bar = addShape("filer_bar", FilerBarShape,
                               FloatRect{8, 8, 610, 30},
                               8, Origin::TopLeft,
                               Color{49, 121, 159},
                               Color{255, 255, 255},
                               2, 0);
    Group filerText = addColor("text", FilerBarText,
                                   Color{240, 226, 107},
                                   Color{0, 0, 0},
                                   2);
    filer_bar.addGroup(filerText);
    addGroup(filer_bar);

    // filer
    Group filer = addShape("filer", FilerShape,
                           FloatRect{9, 46, 314, 372},
                           8, Origin::TopLeft,
                           Color{204, 228, 240},
                           Color{49, 121, 159},
                           3, 0);
    filer.addOption({"file_color", Color{97, 190, 236}});
    filer.addOption({"dir_color", Color{49, 121, 159}});
    // filer highlight
    Group filerHighlight = addShape("highlight", FilerHighlightShape,
                                    FloatRect{1, 0, 312, 26},
                                    8, Origin::TopLeft,
                                    Color{240, 226, 107},
                                    Color{49, 121, 159},
                                    2, 0);
    filer.addGroup(filerHighlight);
    addGroup(filer);

    // preview image
    Group preview_image = addShape("preview_image", PreviewImageShape,
                                   FloatRect{482, 155, 190, 190},
                                   8, Origin::Center,
                                   Color{204, 228, 240},
                                   Color{228, 81, 72},
                                   6, 1);
    addGroup(preview_image);

    // preview video
    Group preview_video = addShape("preview_video", PreviewVideoShape,
                                   FloatRect{482, 367, 256, 192},
                                   8, Origin::Center,
                                   Color{204, 228, 240},
                                   Color{49, 121, 159},
                                   6, 1);
    addGroup(preview_video);

    // help box
    Group help = addShape("help", HelpShape,
                          FloatRect{478, 387, 0, 0},
                          8, Origin::Center,
                          Color{178, 226, 249},
                          Color{49, 121, 159},
                          2, 0);
    addGroup(help);

    // load the configuration from file, overwriting default values
    if (!load()) {
#if 1
        // file doesn't exist or is malformed, (re)create it
        printf("Skin: skin configuration file doesn't exist or is malformed, "
               "creating a new one (%s)\n", getPath().c_str());
        if (!save()) {
            printf("Skin: could not create skin configuration file (%s)\n", getPath().c_str());
        }
#endif
    }
}

config::Group Skin::addShape(const std::string &name, int id,
                             const c2d::FloatRect &rect, int corners_radius, const c2d::Origin &origin,
                             const c2d::Color &color, const c2d::Color &outlineColor, int outlineSize,
                             int tween_type) {
    config::Group group(name, id);
    group.addOption({"rect", rect});
    group.addOption({"corners_radius", corners_radius});
    group.addOption({"origin", (int) origin});
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    group.addOption({"tween_type", tween_type});
    return group;
}

Skin::CustomShape Skin::getShape(int groupId) {

    CustomShape shape;

    Group *group = getGroup(groupId);
    if (!group) {
        printf("Skin::getShape(%i): group not found...\n", groupId);
        return shape;
    }

    shape.rect = group->getOption("rect")->getFloatRect();
    shape.corners_radius = group->getOption("corners_radius")->getInteger();
    shape.origin = (Origin) group->getOption("origin")->getInteger();
    shape.color = group->getOption("color")->getColor();
    shape.outlineColor = group->getOption("outline_color")->getColor();
    shape.outlineSize = (float) group->getOption("outline_size")->getInteger();
    shape.tweenType = (TweenType) group->getOption("tween_type")->getInteger();
    if (groupId == FilerShape) {
        shape.colorFile = group->getOption("file_color")->getColor();
        shape.colorDir = group->getOption("dir_color")->getColor();
    }

    return shape;
}

config::Group
Skin::addColor(const std::string &name, int id,
               const Color &color, const Color &outlineColor, int outlineSize) {
    config::Group group(name, id);
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    return group;
}

Skin::CustomColor Skin::getColor(int groupId) {
    CustomColor colors;

    Group *group = getGroup(groupId);
    if (!group) {
        printf("Skin::getColor(%i): group not found...\n", groupId);
        return colors;
    }

    colors.color = group->getOption("color")->getColor();
    colors.outlineColor = group->getOption("outline_color")->getColor();
    colors.outlineSize = (float) group->getOption("outline_size")->getInteger();

    return colors;
}
