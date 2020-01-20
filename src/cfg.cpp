//
// Created by cpasjuste on 20/01/2020.
//

#include "cfg.h"

using namespace c2d;
using namespace c2d::config;

RetroConfig::RetroConfig(const std::string &path) : Config("RetroDream", path) {

    Group main("MAIN");
    main.addOption({"FILER_LAST_PATH", "/"});
    addGroup(main);

    // load the configuration from file, overwriting default values (added in addConfigGroup)
    if (!load()) {
        // file doesn't exist or is malformed, (re)create it
        save();
    }
}
