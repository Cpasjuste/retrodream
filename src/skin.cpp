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
    // items text
    filer.addOption({"text_spacing", 8, FilerTextSpacing});
    filer.addGroup(addText("text_file", FilerFileText,
                           Color{97, 190, 236}, FONT_SIZE));
    filer.addGroup(addText("text_dir", FilerDirText,
                           Color{49, 121, 159}, FONT_SIZE));
    // highlight
    Group filerHighlight = addShape("highlight", FilerHighlightShape,
                                    FloatRect{1, 0, 368, 26},
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
    filer_path.addGroup(addText("text", FilerBarText,
                                Color{240, 226, 107}, FONT_SIZE));
    filer.addGroup(filer_path);
    //
    addGroup(filer);
    /// filer

    // preview image
    Group preview_image = addShape("preview_image", PreviewImageShape,
                                   FloatRect{505, 161, 228, 228},
                                   8, Origin::Center,
                                   Color{204, 228, 240},
                                   Color{49, 121, 159},
                                   4, 1);
    addGroup(preview_image);

    // preview video
    Group preview_video = addShape("preview_video", PreviewVideoShape,
                                   FloatRect{505, 375, 228, 171},
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
    menu.addGroup(addText("text_left", MenuLeftText,
                          Color{49, 121, 159}, FONT_SIZE));
    menu.addGroup(addText("text_right", MenuRightText,
                          Color{255, 255, 255}, FONT_SIZE));
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
#if 0
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

config::Group
Skin::addText(const std::string &name, int id,
              const Color &color, int size) {
    config::Group group(name, id);
    group.addOption({"color", color});
    group.addOption({"size", size});
    return group;
}

Skin::CustomColor Skin::getColor(int groupId) {
    CustomColor customColor;

    Group *group = getGroup(groupId);
    if (!group) {
        printf("Skin::getColor(%i): group not found...\n", groupId);
        return customColor;
    }

    customColor.color = group->getOption("color")->getColor();
    customColor.outlineColor = group->getOption("outline_color")->getColor();
    customColor.outlineSize = (float) group->getOption("outline_size")->getInteger();

    return customColor;
}

Skin::CustomText Skin::getText(int groupId) {
    CustomText customText;

    Group *group = getGroup(groupId);
    if (!group) {
        printf("Skin::getText(%i): group not found...\n", groupId);
        return customText;
    }

    customText.color = group->getOption("color")->getColor();
    customText.size = group->getOption("size")->getInteger();

    return customText;
}
