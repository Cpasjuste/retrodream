//
// Created by cpasjuste on 20/01/2020.
//

#include "cross2d/c2d.h"
#include "cfg.h"
#include "retroio.h"

using namespace c2d;
using namespace c2d::config;

RetroConfig::RetroConfig(RetroIo *retroIo) : Config("RetroDreamConfig", retroIo->getConfigPath()) {

    io = retroIo;

    Group main("main");
    main.addOption({"filer_path", io->getHomePath(), OptionId::FilerPath});
    main.addOption({"screen_size", FloatRect{0, 0, 640, 480}, OptionId::ScreenSize});
    main.addOption({"input_delay", 200, OptionId::InputDelay});
    main.addOption({"image_delay", 500, OptionId::PreviewImageDelay});
    main.addOption({"video_delay", 3000, OptionId::PreviewVideoDelay});
    addGroup(main);

    /// SKIN
    Group skin("skin");
    // filer
    Group filer("filer", FilerShape);
    filer.addOption({"rect", FloatRect{9, 46, 314, 372}, FilerShape});
    filer.addOption({"color", Color{204, 228, 240}, FilerShape + 1});
    filer.addOption({"outline_color", Color{49, 121, 159}, FilerShape + 2});
    filer.addOption({"outline_size", 3, FilerShape + 3});
    filer.addOption({"tween_type", 0, FilerShape + 4});
    filer.addOption({"file_color", Color{97, 190, 236}, FilerShape + 5});
    filer.addOption({"dir_color", Color{49, 121, 159}, FilerShape + 6});
    skin.addGroup(filer);
    // preview image
    Group preview_image("preview_image", PreviewImageShape);
    preview_image.addOption({"rect", FloatRect{477, 160, 206, 206}, PreviewImageShape});
    preview_image.addOption({"color", Color{204, 228, 240}, PreviewImageShape + 1});
    preview_image.addOption({"outline_color", Color{228, 81, 72}, PreviewImageShape + 2});
    preview_image.addOption({"outline_size", 6, PreviewImageShape + 3});
    preview_image.addOption({"tween_type", 0, PreviewImageShape + 4});
    skin.addGroup(preview_image);
    // preview video
    Group preview_video("preview_video", PreviewVideoShape);
    preview_video.addOption({"rect", FloatRect{478, 366, 256, 192}, PreviewVideoShape});
    preview_video.addOption({"color", Color{204, 228, 240}, PreviewVideoShape + 1});
    preview_video.addOption({"outline_color", Color{240, 226, 107}, PreviewVideoShape + 2});
    preview_video.addOption({"outline_size", 3, PreviewVideoShape + 3});
    preview_video.addOption({"tween_type", 0, PreviewVideoShape + 4});
    skin.addGroup(preview_video);
    // help box
    Group help("help", HelpShape);
    help.addOption({"rect", FloatRect{478, 387, 0, 0}, HelpShape});
    help.addOption({"color", Color{178, 226, 249}, HelpShape + 1});
    help.addOption({"outline_color", Color{49, 121, 159}, HelpShape + 2});
    help.addOption({"outline_size", 2, HelpShape + 3});
    help.addOption({"tween_type", 0, HelpShape + 4});
    skin.addGroup(help);
    // add all
    addGroup(skin);
    /// SKIN

    // load the configuration from file, overwriting default values
    if (!load()) {
        // file doesn't exist or is malformed, (re)create it
        printf("RetroConfig: config file doesn't exist or is malformed, "
               "creating a new one (%s)\n", getPath().c_str());
        // TODO: enable back...
        /*
        if (!save()) {
            printf("RetroConfig: could not create configuration file (%s)\n", getPath().c_str());
        }
        */
    }

    // check default filer data directory
    std::string filerPath = get(FilerPath);
    if (!io->exist(filerPath)) {
        printf("RetroConfig: FilerPath '%s' doesn't exist, restoring default: '%s'\n",
               filerPath.c_str(), io->getHomePath().c_str());
        filerPath = io->getHomePath();
        set(FilerPath, filerPath, true);
    }

    printf("RetroConfig: retrodream path: %s\n", io->getDataPath().c_str());
    printf("RetroConfig: retrodream config path: %s\n", io->getConfigPath().c_str());
    printf("RetroConfig: dreamshell path: %s\n", io->getDsPath().c_str());
    printf("RetroConfig: dreamshell binary path: %s\n", io->getDsBinPath().c_str());
    printf("RetroConfig: filer path: %s\n", filerPath.c_str());
}

std::string RetroConfig::get(const RetroConfig::OptionId &id) {
    return getGroup("main")->getOption(id)->getString();
}

void RetroConfig::set(const RetroConfig::OptionId &id, const std::string &value, bool s) {
    getGroup("main")->getOption(id)->setString(value);
    if (s) {
        save();
    }
}

int RetroConfig::getInt(const RetroConfig::OptionId &id) {
    return getGroup("main")->getOption(id)->getInteger();
}

void RetroConfig::setInt(const RetroConfig::OptionId &id, int value, bool s) {
    getGroup("main")->getOption(id)->setInteger(value);
    if (s) {
        save();
    }
}

c2d::FloatRect RetroConfig::getRect(const OptionId &id) {
    FloatRect rect = getGroup("main")->getOption(id)->getFloatRect();
    return rect;
}

void RetroConfig::setRect(const OptionId &id, const c2d::FloatRect &rect, bool s) {
    getGroup("main")->getOption(id)->setFloatRect(rect);
    if (s) {
        save();
    }
}

RetroConfig::CustomShape RetroConfig::getShape(int groupId) {

    CustomShape shape;

    Group *group = getGroup(groupId);
    if (!group) {
        printf("RetroConfig::getShape(%i): group not found...\n", groupId);
        return shape;
    }

    shape.rect = group->getOption(groupId)->getFloatRect();
    shape.color = group->getOption(groupId + 1)->getColor();
    shape.outlineColor = group->getOption(groupId + 2)->getColor();
    shape.outlineSize = (float) group->getOption(groupId + 3)->getInteger();
    shape.tweenType = (TweenType) group->getOption(groupId + 4)->getInteger();
    if (groupId == FilerShape) {
        shape.colorFile = group->getOption(groupId + 5)->getColor();
        shape.colorDir = group->getOption(groupId + 6)->getColor();
    }

    return shape;
}
