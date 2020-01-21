//
// Created by cpasjuste on 20/01/2020.
//

#include "cross2d/c2d.h"
#include "cfg.h"
#include "retroio.h"

using namespace c2d;
using namespace c2d::config;

RetroConfig::RetroConfig(c2d::Io *retroIo) : Config("RetroDreamConfig", ((RetroIo *) retroIo)->getConfigPath()) {

    io = (RetroIo *) retroIo;

    Group main("main");
    main.addOption({"data_path", io->getDataPath()});
    main.addOption({"filer_last_path", io->getHomePath()});
    addGroup(main);

    // load the configuration from file, overwriting default values
    if (!load()) {
        // file doesn't exist or is malformed, (re)create it
        save();
    }

    std::string dataPath = getDataPath();
    io->setDataPath(dataPath);
}

std::string RetroConfig::getDataPath() {

    std::string path = getGroup("main")->getOption("data_path")->getString();
    if (!io->exist(path)) {
        printf("RetroConfig::getDataPath(): '%s' doesn't exist, restoring default: '%s'\n",
               path.c_str(), io->getDataPath().c_str());
        path = io->getDataPath();
        getGroup("main")->getOption("data_path")->setString(path);
        save();
    }

    return path;
}

std::string RetroConfig::getLastPath() {

    std::string path = getGroup("main")->getOption("filer_last_path")->getString();
    if (!io->exist(path)) {
        printf("RetroConfig::getLastPath(): '%s' doesn't exist, restoring default: '%s'\n",
               path.c_str(), io->getHomePath().c_str());
        path = io->getHomePath();
        getGroup("main")->getOption("filer_last_path")->setString(path);
        save();
    }

    return path;
}

void RetroConfig::setLastPath(const std::string &path) {

    if (io->exist(path)) {
        getGroup("main")->getOption("filer_last_path")->setString(path);
        save();
    }
}
