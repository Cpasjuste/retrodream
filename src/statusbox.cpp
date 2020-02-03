//
// Created by cpasjuste on 08/12/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "statusbox.h"

using namespace c2d;

StatusBox::StatusBox(RetroDream *rd, const c2d::FloatRect &rect) : Rectangle(rect) {

    retroDream = rd;

    icon = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/wait.png");
    icon->setOrigin(Origin::Center);
    float scaling = std::min(
            rect.width / (float) (icon->getTextureRect().width + 4),
            rect.height / (float) (icon->getTextureRect().height + 4));
    icon->setScale(scaling, scaling);
    icon->setPosition(rect.height / 2, rect.height / 2);
    icon->setFillColor(COL_RED);
    iconTween = new TweenRotation(0, 360, 2, TweenLoop::Loop);
    icon->add(iconTween);
    add(icon);

    titleText = new Text("", FONT_SIZE);
    titleText->setFillColor(COL_RED);
    titleText->setOutlineColor(Color::Black);
    titleText->setOutlineThickness(1);
    titleText->setPosition(icon->getPosition().x + (icon->getSize().x / 2) - 8, 0);
    add(titleText);

    messageText = new Text("", FONT_SIZE);
    messageText->setScale(0.85f, 0.85f);
    messageText->setFillColor(COL_RED);
    messageText->setOutlineColor(Color::Black);
    messageText->setOutlineThickness(1);
    messageText->setPosition(titleText->getPosition().x, titleText->getPosition().y + FONT_SIZE + 8);
    messageText->setSizeMax(rect.width - icon->getSize().x + 10, 0);
    add(messageText);

    add(new TweenAlpha(0, 255, 0.5f));
    setVisibility(Visibility::Hidden);
}

void StatusBox::show(const std::string &title, const std::string &message,
                     const c2d::Color &color, bool inf) {

    if (messageText == nullptr) {
        return;
    }

    infinite = inf;
    titleText->setString(Utility::toUpper(title));
    messageText->setString(Utility::toUpper(message));

    Color c = icon->getFillColor();
    if (c != color) {
        titleText->setFillColor(color);
        messageText->setFillColor(color);
        icon->setFillColor(color);
    }

    if (!isVisible()) {
        setVisibility(Visibility::Visible, true);
        iconTween->play();
    }

    clock.restart();
}

void StatusBox::hide() {
    clock.restart();
    infinite = false;
}

void StatusBox::onUpdate() {

    if (isVisible() && !infinite && clock.getElapsedTime().asSeconds() > 2) {
        setVisibility(Visibility::Hidden, true);
    }

    Rectangle::onUpdate();
}
