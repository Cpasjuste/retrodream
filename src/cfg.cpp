//
// Created by cpasjuste on 20/01/2020.
//

#include "cross2d/c2d.h"
#include "cfg.h"
#include "retroio.h"
#include "filer.h"
#include "utility.h"

using namespace c2d;
using namespace c2d::config;

RetroConfig::RetroConfig(c2d::Io *retroIo) : Config("RetroDreamConfig", ((RetroIo *) retroIo)->getConfigPath()) {

    io = (RetroIo *) retroIo;
    FloatRect screenSize(0, 0, 640, 480);

    Group main("main");

    main.addOption({"data_path", io->getDataPath(), OptionId::DataPath});
    main.addOption({"ds_path", RetroUtility::findPath(io, "DS/"), OptionId::DsPath});
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

    // ensure all paths exist
    bool saveNeeded = false;
    std::string path = getGroup("main")->getOption(DataPath)->getString();
    if (!io->exist(path)) {
        printf("RetroConfig::DataPath '%s' doesn't exist, restoring default: '%s'\n",
               path.c_str(), io->getDataPath().c_str());
        path = io->getDataPath();
        io->create(path);
        set(DataPath, path, false);
        saveNeeded = true;
    }
    path = getGroup("main")->getOption(LastPath)->getString();
    if (!io->exist(path)) {
        printf("RetroConfig::LastPath: '%s' doesn't exist, restoring default: '%s'\n",
               path.c_str(), io->getHomePath().c_str());
        path = io->getHomePath();
        set(LastPath, path, false);
        saveNeeded = true;
    }
    path = getGroup("main")->getOption(DsPath)->getString();
    if (!io->exist(path)) {
        std::string newPath = RetroUtility::findPath(io, "DS/");
        printf("RetroConfig::DsPath: '%s' doesn't exist, restoring default: '%s'\n",
               path.c_str(), newPath.c_str());
        path = io->getHomePath();
        set(DsPath, newPath, false);
        saveNeeded = true;
    }

    if (saveNeeded) {
        save();
    }

    io->setDataPath(get(DataPath));
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
