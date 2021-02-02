//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "presetmenu.h"

using namespace c2d;

PresetMenu::PresetMenu(RetroDream *rd, const c2d::FloatRect &rect) : Menu(rd, rect) {

    title->setString("GAME OPTIONS");

    config.addOption({"BOOT:", {"DIRECT", "IP.BIN", "IP.BIN (CUT)"}, 0, Mode});
    config.addOption({"MEMORY:", getAddresses(), 0, Memory});
    config.addOption({"DMA:", {"OFF", "ON"}, 0, Dma});
    config.addOption({"SYNC:", {"TRUE", "1", "2", "3", "4", "5", "6", "7", "8", "16"}, 8, Async});
    config.addOption({"CDDA:", {"OFF", "ON"}, 0, Cdda});
    config.addOption({"OS:", {"AUTO", "HOMEBREW", "KATANA", "WINCE"}, 0, Type});
    config.addOption({"LOADER:", {"AUTO", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                                  "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"}, 0, Device});
    configBox->load(&config);
}

void PresetMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {
        Filer::RetroFile file = retroDream->getFiler()->getSelection();
        if (file.isGame) {
            isoLoaderConfig = IsoLoader::loadConfig(retroDream, file.isoPath);
            config.getOption(Mode)->setChoicesIndex(isoLoaderConfig.mode);
            config.getOption(Memory)->setChoicesIndex(Utility::toUpper(isoLoaderConfig.memory));
            config.getOption(Dma)->setChoicesIndex(isoLoaderConfig.dma);
            if (isoLoaderConfig.async < 1) {
                config.getOption(Async)->setChoicesIndex(0);
            } else {
                config.getOption(Async)->setChoicesIndex(Utility::toString(isoLoaderConfig.async));
            }
            config.getOption(Cdda)->setChoicesIndex(isoLoaderConfig.cdda);
            if (isoLoaderConfig.device == "auto") {
                config.getOption(Device)->setChoicesIndex(0);
            } else {
                config.getOption(Device)->setChoicesIndex(isoLoaderConfig.device);
            }
            config.getOption(Type)->setChoicesIndex(isoLoaderConfig.type);
            configBox->load(&config);
        }
    } else {
        save();
    }

    Menu::setVisibility(visibility, tweenPlay);
}

void PresetMenu::save() {
    if (dirty) {
        isoLoaderConfig.device = Utility::toLower(config.getOption(Device)->getString());
        isoLoaderConfig.dma = config.getOption(Dma)->getChoiceIndex();
        isoLoaderConfig.cdda = config.getOption(Cdda)->getChoiceIndex();
        isoLoaderConfig.mode = config.getOption(Mode)->getChoiceIndex();
        isoLoaderConfig.type = config.getOption(Type)->getChoiceIndex();
        isoLoaderConfig.memory = Utility::toLower(config.getOption(Memory)->getString());
        if (config.getOption(Async)->getChoiceIndex() == 0) {
            isoLoaderConfig.async = 0;
        } else {
            isoLoaderConfig.async = Utility::parseInt(config.getOption(Async)->getString());
        }
        if (!IsoLoader::saveConfig(retroDream, isoLoaderConfig)) {
            retroDream->showStatus("PRESET ERROR",
                                   "COULD NOT WRITE TO " + Utility::toUpper(isoLoaderConfig.path));
        }
        dirty = false;
    }
}

std::vector<std::string> PresetMenu::getAddresses() {

    return {
            "0X8C004000",
            "0X8C000100",
            "0X8C004800",
            "0X8C008000",
            "0x8CB00000",
            "0X8CE00000",
            "0X8CEF8000",
            "0X8CF80000",
            "0X8CFA0000",
            "0X8CFC0000",
            "0X8CFD0000",
            "0X8CFE0000",
            "0X8CFE8000",
            "0X8CFF0000",
            "0X8CFF4800",
            "0X8CFF6000"
    };
}
