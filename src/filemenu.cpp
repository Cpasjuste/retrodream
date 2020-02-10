//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"

using namespace c2d;

FileMenu::FileMenu(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    retroDream = rd;

    title = new Text("FILE OPTIONS", FONT_SIZE);
    title->setOrigin(Origin::BottomLeft);
    title->setPosition(16, 0);
    title->setOutlineThickness(3);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    FloatRect configRect = {16, 16, rect.width - 28, rect.height - 64};
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect, FONT_SIZE + 10);
    mainConfig.addOption({"COPY", "GO", Copy});
    mainConfig.addOption({"PASTE", "GO", Paste});
    mainConfig.addOption({"DELETE", "GO", Delete});
    configBox->load(&mainConfig);

    configBox->getListBoxLeft()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxLeft()->setTextOutlineColor(Color::Black);
    configBox->getListBoxLeft()->setTextOutlineThickness(2);
    configBox->getListBoxLeft()->setTextColor(COL_BLUE_DARK);

    configBox->getListBoxRight()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxRight()->setTextOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->setTextOutlineThickness(2);
    configBox->getListBoxRight()->setTextColor(Color::White);
    configBox->getListBoxRight()->getHighlight()->setFillColor(COL_YELLOW);
    configBox->getListBoxRight()->getHighlight()->setOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->getHighlight()->setOutlineThickness(1);

    add(configBox);

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.2f));
    setVisibility(Visibility::Hidden);
}

void FileMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {
        configBox->reset();
        retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible, true);
        retroDream->getFiler()->setSelectionFront();
    } else {
        retroDream->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
        retroDream->getFiler()->setSelectionBack();
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
        dirty = true;
    } else if (keys & Input::Key::Left) {
        configBox->navigate(ConfigBox::Navigation::Left);
        dirty = true;
    } else if (keys & Input::Key::Fire1) {
        auto option = configBox->getSelection();
        if (option != nullptr) {
            if (option->getId() == Copy) {
                file = retroDream->getFiler()->getSelection();
                if (file.data.name != "..") {
                    setVisibility(Visibility::Hidden, true);
                    retroDream->showStatus("COPY", Utility::toUpper(file.data.path), COL_GREEN);
                } else {
                    file = {};
                }
            } else if (option->getId() == Paste) {
                if (!file.data.path.empty()) {
                    setVisibility(Visibility::Hidden, true);
                    retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible);
                    retroDream->getProgressBox()->setTitle("COPY IN PROGRESS");
                    retroDream->getProgressBox()->setMessage(
                            "\n\n\nCOPYING STUFF, PLEASE WAIT...");
                    retroDream->getProgressBox()->setProgress("LOADING STUFF.... \n", 0.0f);
                    retroDream->getProgressBox()->setVisibility(Visibility::Visible);
                    RetroDream *rd = retroDream;
                    retroDream->getRender()->getIo()->copy(
                            file.data.path, retroDream->getFiler()->getPath(),
                            [rd](const Io::File &src, const Io::File &dst, float progress) {
                                if (progress < 0 && rd->getProgressBox()->isVisible()) {
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getMessageBox()->getTitleText()->setFillColor(COL_RED);
                                    rd->getMessageBox()->show(
                                            "COPY - ERROR",
                                            "\n\n\nAN ERROR OCCURRED WHILE COPYING FILES...", "OK");
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                                } else if (progress > 1) {
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                                    rd->getFiler()->getDir(rd->getFiler()->getPath());
                                } else {
                                    rd->getProgressBox()->setProgress(Utility::toUpper(dst.name), progress);
                                    rd->getRender()->flip(true, false);
                                }
                            });
                }
                file = {};
            } else if (option->getId() == Delete) {
                if (retroDream->getFiler()->getSelection().data.name != "..") {
                    file = retroDream->getFiler()->getSelection();
                    setVisibility(Visibility::Hidden, true);
                    int res = retroDream->getMessageBox()->show(
                            "DELETE",
                            "\n\nDO YOU REALLY WANT TO DELETE: \n\n" + file.upperName,
                            "CANCEL", "CONFIRM");
                    if (res == MessageBox::RIGHT) {
                        if (file.data.type == Io::Type::Directory) {
                            res = retroDream->getRender()->getIo()->removeDir(file.data.path);
                        } else {
                            res = retroDream->getRender()->getIo()->removeFile(file.data.path);
                        }
                        if (res) {
                            setVisibility(Visibility::Hidden, true);
                            retroDream->getFiler()->getDir(retroDream->getFiler()->getPath());
                        } else {
                            retroDream->getMessageBox()->show(
                                    "DELETE - ERROR",
                                    "\n\n\nAN ERROR OCCURRED WHILE DELETING FILES...", "OK");
                        }
                    }
                    file = {};
                }
            }
        }
    } else if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
    }

    return true;
}
