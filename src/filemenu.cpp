//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "filemenu.h"
#include "colors.h"

using namespace c2d;

FileMenu::FileMenu(RetroDream *rd, Skin::CustomShape *shape) : Menu(rd, shape) {

    title->setString("FILE OPTIONS");
}

void FileMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {
        config.getOptions()->clear();
        Filer::RetroFile f = retroDream->getFiler()->getSelection();
        if (f.data.type == Io::Type::Directory) {
            if (f.isGame) {
                config.addOption({"BROWSE", "GO", Browse});
            } else if (RetroUtility::getVmuDevice(f.data.path) != nullptr) {
                config.addOption({"BACKUP VMU", "GO", VmuBackup});
            }
        }
        config.addOption({"COPY", "GO", Copy});
        config.addOption({"PASTE", "GO", Paste});
        config.addOption({"DELETE", "GO", Delete});
        configBox->load(&config);
    }

    Menu::setVisibility(visibility, tweenPlay);
}

bool FileMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].buttons;

    if (keys & Input::Button::A) {
        auto option = configBox->getSelection();
        if (option != nullptr) {
            if (option->getId() == Browse) {
                retroDream->getFiler()->getDir(retroDream->getFiler()->getSelection().data.path);
                setVisibility(Visibility::Hidden, true);
            } else if (option->getId() == VmuBackup) {
                retroDream->getProgressBox()->setTitle("VMU BACKUP IN PROGRESS");
                retroDream->getProgressBox()->setMessage("\nDOING RAW VMU BACKUP...");
                retroDream->getProgressBox()->setProgress("PLEASE WAIT.... \n", 0.0f);
                retroDream->getProgressBox()->setVisibility(Visibility::Visible);
                RetroDream *rd = retroDream;
                Filer::RetroFile f = retroDream->getFiler()->getSelection();
                RetroUtility::vmuBackup(
                        f.data.path, rd->getIo()->getDataPath(),
                        [rd](const std::string &msg, float progress) {
                            if (progress < 0) {
                                rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                rd->showStatus("VMU BACKUP ERROR", msg);
                            } else if (progress > 1) {
                                rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                rd->showStatus("VMU BACKUP SUCCESS", "VMU SAVED TO " + msg, COL_GREEN);
                            } else {
                                rd->getProgressBox()->setProgress(msg, progress);
                                rd->getRender()->flip(true, false);
                            }
                        });
            } else if (option->getId() == Copy) {
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
                            "\nCOPYING STUFF, PLEASE WAIT...");
                    retroDream->getProgressBox()->setProgress("LOADING STUFF.... \n", 0.0f);
                    retroDream->getProgressBox()->setVisibility(Visibility::Visible);
                    RetroDream *rd = retroDream;
                    rd->getPreviewVideo()->unload();
                    rd->getRender()->getIo()->copy(
                            file.data.path, rd->getFiler()->getPath(),
                            [rd](const Io::File &src, const Io::File &dst, float progress) {
                                if (progress < 0 && rd->getProgressBox()->isVisible()) {
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getMessageBox()->getTitleText()->setFillColor(COL_RED);
                                    rd->getMessageBox()->show(
                                            "COPY - ERROR",
                                            "AN ERROR OCCURRED WHILE COPYING FILES...", "OK");
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                                } else if (progress > 1) {
                                    rd->getProgressBox()->setVisibility(Visibility::Hidden);
                                    rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                                    rd->getFiler()->getDir(rd->getFiler()->getPath());
                                } else {
                                    rd->getProgressBox()->setProgress(Utility::toUpper(dst.name), progress);
                                    rd->getRender()->flip(true, false);
                                    rd->getPreviewVideo()->unload();
                                }
                            });
                }
                file = {};
            } else if (option->getId() == Delete) {
                if (retroDream->getFiler()->getSelection().data.name != "..") {
                    file = retroDream->getFiler()->getSelection();
                    setVisibility(Visibility::Hidden, true);
                    int res = retroDream->getMessageBox()->show(
                            "CONFIRM DELETION",
                            "DO YOU REALLY WANT TO DELETE: \n\"" + file.upperName + "\"",
                            "CANCEL", "CONFIRM");
                    if (res == MessageBox::RIGHT) {
                        retroDream->getPreviewVideo()->unload();
                        if (file.data.type == Io::Type::Directory) {
                            res = (int) retroDream->getRender()->getIo()->removeDir(file.data.path);
                        } else {
                            res = (int) retroDream->getRender()->getIo()->removeFile(file.data.path);
                        }
                        if (res == 1) {
                            setVisibility(Visibility::Hidden, true);
                            retroDream->getFiler()->getDir(retroDream->getFiler()->getPath());
                        } else {
                            retroDream->getMessageBox()->show(
                                    "DELETE - ERROR",
                                    "AN ERROR OCCURRED WHILE DELETING FILES...", "OK");
                        }
                    }
                    file = {};
                }
            }
        }
    }

    return Menu::onInput(players);
}
