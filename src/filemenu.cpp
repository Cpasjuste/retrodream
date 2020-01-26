//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filemenu.h"
#include "isoloader.h"

using namespace c2d;

const std::vector<std::string> FileMenuMemAddr = {
        "0X8C004000",
        "0X8C000100",
        "0X8C004800",
        "0X8C008000",
        "0X8CEf8000",
        "0X8CF80000",
        "0X8CFE0000",
        "0X8CFE8000",
        "0X8CFD0000",
        "0X8CFF0000",
        "0X8CFF4800",
        "0X8CFF6000",
        "0X8CE00000",
        "0X8C008000",
        "0X8CFA0000",
        "0X8CFC0000",
        "0X8CFD0000"
};

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
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect);
    configGroup.addOption({"BOOT:", {"DIRECT", "IP.BIN", "IP.BIN (TRUNC)"}, 0, Boot});
    configGroup.addOption({"MEMORY:", FileMenuMemAddr, 0, Memory});
    configGroup.addOption({"DMA:", {"OFF", "ON"}, 0, Dma});
    configGroup.addOption({"SYNC:", {"OFF", "1", "2", "3", "4", "5", "6", "7", "8", "16"}, 0, Sync});
    configGroup.addOption({"CDDA:", {"OFF", "ON"}, 0, Cdda});
    configGroup.addOption({"LOADER:", {"AUTO", "CD", "SD", "IDE", "PC", "5", "6", "7", "8"}, 0, Loader});
    configGroup.addOption({"OS:", {"AUTO", "HOMEBREW", "KATANA", "WINCE"}, 0, Os});
    configBox->load(&configGroup);

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
            IsoLoader::Config config = IsoLoader::loadConfig(retroDream, file.isoPath);
            configGroup.getOption(Boot)->setChoicesIndex(config.mode);
            configGroup.getOption(Memory)->setChoicesIndex(config.memory);
            configGroup.getOption(Dma)->setChoicesIndex(config.dma);
            if (config.async < 1) {
                configGroup.getOption(Sync)->setChoicesIndex(0);
            } else {
                configGroup.getOption(Sync)->setChoicesIndex(Utility::toString(config.async));
            }
            configGroup.getOption(Cdda)->setChoicesIndex(config.cdda);
            if (config.device == "auto") {
                configGroup.getOption(Loader)->setChoicesIndex(0);
            } else {
                configGroup.getOption(Loader)->setChoicesIndex(config.device);
            }
            configGroup.getOption(Os)->setChoicesIndex(config.type);
            configBox->load(&configGroup);
        } else {
            // TODO: handle files operations
        }
    }

    RoundedRectangleShape::setVisibility(visibility, tweenPlay);
}

bool FileMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        configBox->navigate(ConfigBox::Navigation::Up);
    } else if (keys & Input::Key::Down) {
        configBox->navigate(ConfigBox::Navigation::Down);
    } else if (keys & Input::Key::Right) {
        configBox->navigate(ConfigBox::Navigation::Right);
    } else if (keys & Input::Key::Left) {
        configBox->navigate(ConfigBox::Navigation::Left);
    } else if (keys & Input::Key::Fire2) {
        // TODO: save
    }

    return true;
}
