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
    main.addOption({"input_delay", 200, OptionId::InputDelay});
    main.addOption({"image_delay", 500, OptionId::PreviewImageDelay});
    main.addOption({"video_delay", 3000, OptionId::PreviewVideoDelay});
    main.addOption({"image_path", "box-2D", OptionId::PreviewImagePath});
    main.addOption({"video_path", "video-normalized", OptionId::PreviewVideoPath});
    addGroup(main);

    // load the configuration from file, overwriting default values
#if 1
    load();
    save();
#endif

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

