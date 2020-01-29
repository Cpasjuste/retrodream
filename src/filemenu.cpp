//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filemenu.h"

using namespace c2d;

FileMenu::FileMenu(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    retroDream = rd;

    title = new Text("FILE OPTION", FONT_SIZE);
    title->setOrigin(Origin::Left);
    title->setPosition(16, -16);
    title->setOutlineThickness(4);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    FloatRect configRect = {16, 16, rect.width - 32, rect.height - 64};
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect, FONT_SIZE + 10);
    presetConfig.addOption({"BOOT:", {"DIRECT", "IP.BIN", "IP.BIN (CUT)"}, 0, Mode});
    presetConfig.addOption({"MEMORY:", getAddresses(), 0, Memory});
    presetConfig.addOption({"DMA:", {"OFF", "ON"}, 0, Dma});
    presetConfig.addOption({"SYNC:", {"OFF", "1", "2", "3", "4", "5", "6", "7", "8", "16"}, 0, Async});
    presetConfig.addOption({"CDDA:", {"OFF", "ON"}, 0, Cdda});
    presetConfig.addOption({"OS:", {"AUTO", "HOMEBREW", "KATANA", "WINCE"}, 0, Type});
    presetConfig.addOption({"LOADER:",
                            {"AUTO", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"},
                            0, Device});
    configBox->load(&presetConfig);

    configBox->getListBoxLeft()->setFillColor(COL_BLUE);
    configBox->getListBoxLeft()->setTextOutlineColor(Color::Black);
    configBox->getListBoxLeft()->setTextOutlineThickness(2);
    configBox->getListBoxLeft()->setTextColor(COL_BLUE_DARK);

    configBox->getListBoxRight()->setFillColor(COL_BLUE);
    configBox->getListBoxRight()->setTextOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->setTextOutlineThickness(2);
    configBox->getListBoxRight()->setTextColor(Color::White);
    configBox->getListBoxRight()->getHighlight()->setFillColor(COL_YELLOW);
    configBox->getListBoxRight()->getHighlight()->setOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->getHighlight()->setOutlineThickness(1);

    add(configBox);

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.3f));
    setVisibility(Visibility::Hidden);
}

void FileMenu::setTitle(const std::string &text) {
    title->setString(text);
}

void FileMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {
        Filer::RetroFile file = retroDream->getFiler()->getSelection();
        if (file.isGame) {
            isoLoaderConfig = IsoLoader::loadConfig(retroDream, file.isoPath);
            if (!retroDream->getRender()->getIo()->exist(isoLoaderConfig.path)) {
                retroDream->showStatus("PRESET NOT FOUND...", isoLoaderConfig.path);
            }
            presetConfig.getOption(Mode)->setChoicesIndex(isoLoaderConfig.mode);
            presetConfig.getOption(Memory)->setChoicesIndex(Utility::toUpper(isoLoaderConfig.memory));
            presetConfig.getOption(Dma)->setChoicesIndex(isoLoaderConfig.dma);
            if (isoLoaderConfig.async < 1) {
                presetConfig.getOption(Async)->setChoicesIndex(0);
            } else {
                presetConfig.getOption(Async)->setChoicesIndex(Utility::toString(isoLoaderConfig.async));
            }
            presetConfig.getOption(Cdda)->setChoicesIndex(isoLoaderConfig.cdda);
            if (isoLoaderConfig.device == "auto") {
                presetConfig.getOption(Device)->setChoicesIndex(0);
            } else {
                presetConfig.getOption(Device)->setChoicesIndex(isoLoaderConfig.device);
            }
            presetConfig.getOption(Type)->setChoicesIndex(isoLoaderConfig.type);
            configBox->load(&presetConfig);
        } else {
            // TODO: handle files operations
        }
    }

    RoundedRectangleShape::setVisibility(visibility, tweenPlay);
}

void FileMenu::save() {
    if (dirty) {
        isoLoaderConfig.device = Utility::toLower(presetConfig.getOption(Device)->getString());
        isoLoaderConfig.dma = presetConfig.getOption(Dma)->getChoiceIndex();
        isoLoaderConfig.cdda = presetConfig.getOption(Cdda)->getChoiceIndex();
        isoLoaderConfig.mode = presetConfig.getOption(Mode)->getChoiceIndex();
        isoLoaderConfig.type = presetConfig.getOption(Type)->getChoiceIndex();
        isoLoaderConfig.memory = Utility::toLower(presetConfig.getOption(Memory)->getString());
        if (presetConfig.getOption(Async)->getString() == "OFF") {
            isoLoaderConfig.async = 0;
        } else {
            isoLoaderConfig.async = Utility::parseInt(presetConfig.getOption(Async)->getString());
        }
        IsoLoader::saveConfig(retroDream, isoLoaderConfig);
        dirty = false;
    }
}

bool FileMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        configBox->navigate(ConfigBox::Navigation::Up);
    } else if (keys & Input::Key::Down) {
        configBox->navigate(ConfigBox::Navigation::Down);
    } else if (keys & Input::Key::Right) {
        configBox->navigate(ConfigBox::Navigation::Right);
        dirty = true;
    } else if (keys & Input::Key::Left) {
        configBox->navigate(ConfigBox::Navigation::Left);
        dirty = true;
    } else if (keys & Input::Key::Fire2) {
        save();
        retroDream->getBlur()->setVisibility(Visibility::Hidden, true);
        setVisibility(Visibility::Hidden, true);
    }

    return true;
}

std::vector<std::string> FileMenu::getAddresses() {

    return {
            "0X8C004000",
            "0X8C000100",
            "0X8C004800",
            "0X8C008000",
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
