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
    main.addOption({"retrodream_path", io->getDataPath(), OptionId::RdPath});
    main.addOption({"dreamshell_path", RetroUtility::findPath(io, "DS/"), OptionId::DsPath});
    main.addOption({"filer_path", io->getHomePath(), OptionId::FilerPath});
    main.addOption({"screen_size", screenSize, OptionId::ScreenSize});
    main.addOption({"input_delay", 200, OptionId::InputDelay});
    addGroup(main);

    // load the configuration from file, overwriting default values
    if (!load()) {
        // file doesn't exist or is malformed, (re)create it
        printf("RetroConfig: config file doesn't exist or is malformed, "
               "creating a new one (%s)\n", getPath().c_str());
        if (!save()) {
            printf("RetroConfig: could not create configuration file (%s)\n", getPath().c_str());
        }
    }

    // ensure all paths exists
    bool saveNeeded = false;

    std::string rdPath = getGroup("main")->getOption(RdPath)->getString();
    if (!Utility::endsWith(rdPath, "/")) {
        rdPath += "/";
    }
    if (!io->exist(rdPath)) {
        printf("RetroConfig: RetroDream path '%s' doesn't exist, restoring default: '%s'\n",
               rdPath.c_str(), io->getDataPath().c_str());
        rdPath = io->getDataPath();
        io->create(Utility::removeLastSlash(rdPath));
        set(RdPath, rdPath, false);
        saveNeeded = true;
    }

    std::string dsPath = getGroup("main")->getOption(DsPath)->getString();
    if (!Utility::endsWith(dsPath, "/")) {
        dsPath += "/";
    }
    if (!io->exist(dsPath)) {
        std::string newPath = RetroUtility::findPath(io, "DS/");
        printf("RetroConfig: DreamShell path '%s' doesn't exist, restoring default: '%s'\n",
               dsPath.c_str(), newPath.c_str());
        dsPath = io->getHomePath();
        set(DsPath, newPath, false);
        saveNeeded = true;
    }

    std::string filerPath = getGroup("main")->getOption(FilerPath)->getString();
    if (!io->exist(filerPath)) {
        printf("RetroConfig: FilerPath '%s' doesn't exist, restoring default: '%s'\n",
               filerPath.c_str(), io->getHomePath().c_str());
        filerPath = io->getHomePath();
        set(FilerPath, filerPath, false);
        saveNeeded = true;
    }

    if (saveNeeded) {
        save();
    }

    printf("retrodream_path: %s\n", rdPath.c_str());
    printf("dreamshell_path: %s\n", dsPath.c_str());
    printf("filer_path: %s\n", filerPath.c_str());
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
