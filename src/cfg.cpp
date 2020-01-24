//
// Created by cpasjuste on 20/01/2020.
//

#include "cross2d/c2d.h"
#include "cfg.h"
#include "retroio.h"
#include "filer.h"

using namespace c2d;
using namespace c2d::config;

RetroConfig::RetroConfig(c2d::Io *retroIo) : Config("RetroDreamConfig", ((RetroIo *) retroIo)->getConfigPath()) {

    io = (RetroIo *) retroIo;
    FloatRect screenSize(0, 0, 640, 480);

    Group main("main");
    main.addOption({"data_path", io->getDataPath(), OptionId::DataPath});
    main.addOption({"filer_last_path", io->getHomePath(), OptionId::LastPath});
    main.addOption({"screen_size", screenSize, OptionId::ScreenSize});
    main.addOption({"input_delay", 200, OptionId::InputDelay});
    main.addOption({"filer_mode", Filer::FilerMode::Browse, OptionId::InputDelay});
    addGroup(main);

    // load the configuration from file, overwriting default values
    if (!load()) {
        // file doesn't exist or is malformed, (re)create it
        save();
    }

    io->setDataPath(get(DataPath));
}

std::string RetroConfig::get(const RetroConfig::OptionId &id) {
    std::string path = getGroup("main")->getOption(id)->getString();
    if (id == DataPath) {
        if (!io->exist(path)) {
            printf("RetroConfig::get(DataPath): '%s' doesn't exist, restoring default: '%s'\n",
                   path.c_str(), io->getDataPath().c_str());
            path = io->getDataPath();
            set(DataPath, path);
        }
    } else if (id == LastPath) {
        if (!io->exist(path)) {
            printf("RetroConfig::get(LastPath): '%s' doesn't exist, restoring default: '%s'\n",
                   path.c_str(), io->getHomePath().c_str());
            path = io->getHomePath();
            set(LastPath, path);
        }
    }

    return path;
}

void RetroConfig::set(const RetroConfig::OptionId &id, const std::string &value) {
    getGroup("main")->getOption(id)->setString(value);
    save();
}

int RetroConfig::getInt(const RetroConfig::OptionId &id) {
    return getGroup("main")->getOption(id)->getInteger();
}

void RetroConfig::setInt(const RetroConfig::OptionId &id, int value) {
    getGroup("main")->getOption(id)->setInteger(value);
    save();
}

c2d::FloatRect RetroConfig::getRect(const OptionId &id) {
    FloatRect rect = getGroup("main")->getOption(id)->getFloatRect();
    return rect;
}

void RetroConfig::setRect(const OptionId &id, const c2d::FloatRect &rect) {
    getGroup("main")->getOption(id)->setFloatRect(rect);
    save();
}
