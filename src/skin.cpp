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

    // logo
    Group logo = addShape("logo", LogoShape,
                          FloatRect{388, 42, 230, 230},
                          0, Origin::TopLeft,
                          Color{255, 255, 255},
                          Color{255, 255, 255},
                          0, 0);
    addGroup(logo);

    /// filer
    Group filer = addShape("filer", FilerShape,
                           FloatRect{9, 46, 370, 415},
                           8, Origin::TopLeft,
                           Color{204, 228, 240},
                           Color{49, 121, 159},
                           3, 0);
    Group filerFileText = addColor("file_text", FilerFileText,
                                   Color{97, 190, 236},
                                   Color{0, 0, 0},
                                   0);
    filer.addGroup(filerFileText);
    Group filerDirText = addColor("dir_text", FilerDirText,
                                  Color{49, 121, 159},
                                  Color{0, 0, 0},
                                  0);
    filer.addGroup(filerDirText);
    // highlight
    Group filerHighlight = addShape("highlight", FilerHighlightShape,
                                    FloatRect{2, 0, 366, 24},
                                    8, Origin::TopLeft,
                                    Color{240, 226, 107},
                                    Color{49, 121, 159},
                                    2, 0);
    filer.addGroup(filerHighlight);
    // filer_path
    Group filer_path = addShape("filer_path", FilerPathShape,
                                FloatRect{8, 8, 610, 30},
                                8, Origin::TopLeft,
                                Color{49, 121, 159},
                                Color{255, 255, 255},
                                2, 0);
    Group filerText = addColor("text", FilerBarText,
                               Color{240, 226, 107},
                               Color{0, 0, 0},
                               0);
    filer_path.addGroup(filerText);
    filer.addGroup(filer_path);
    //
    addGroup(filer);
    /// filer

    // preview image
    Group preview_image = addShape("preview_image", PreviewImageShape,
                                   FloatRect{509, 166, 228, 228},
                                   8, Origin::Center,
                                   Color{204, 228, 240},
                                   Color{49, 121, 159},
                                   4, 1);
    addGroup(preview_image);

    // preview video
    Group preview_video = addShape("preview_video", PreviewVideoShape,
                                   FloatRect{509, 378, 228, 171},
                                   8, Origin::Center,
                                   Color{204, 228, 240},
                                   Color{49, 121, 159},
                                   4, 1);
    addGroup(preview_video);

    // help box
    Group help = addShape("help", HelpShape,
                          FloatRect{508, 288, 233, 172},
                          8, Origin::Top,
                          Color{178, 226, 249},
                          Color{49, 121, 159},
                          3, 0);
    addGroup(help);

    // menus
    Group menu = addShape("menu", MenuShape,
                          FloatRect{334, 46, 314, 282},
                          8, Origin::TopLeft,
                          Color{204, 228, 240},
                          Color{49, 121, 159},
                          3, 0);
    Group menuLeftText = addColor("left_text", MenuLeftText,
                                  Color{49, 121, 159},
                                  Color{0, 0, 0},
                                  0);
    menu.addGroup(menuLeftText);
    Group menuRightText = addColor("right_text", MenuRightText,
                                   Color{255, 255, 255},
                                   Color{49, 121, 159},
                                   0);
    menu.addGroup(menuRightText);
    // highlight
    Group menuHighlight = addShape("highlight", MenuHighlightShape,
                                   FloatRect{1, 0, 312, 26},
                                   8, Origin::TopLeft,
                                   Color{240, 226, 107},
                                   Color{49, 121, 159},
                                   2, 0);
    menu.addGroup(menuHighlight);
    addGroup(menu);

    // load the configuration from file, overwriting default values
#if 1
    load();
    save();
#endif
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
