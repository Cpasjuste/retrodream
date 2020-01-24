//
// Created by cpasjuste on 08/12/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "statusbox.h"

using namespace c2d;

StatusBox::StatusBox(RetroDream *rd, const c2d::FloatRect &rect)
        : Rectangle(rect) {

    retroDream = rd;

    pos = {rect.left, rect.top};

    icon = new C2DTexture(retroDream->getRender()->getIo()->getRomFsPath() + "skin/wait.png");
    icon->setOrigin(Origin::Center);
    float scaling = std::min(
            rect.width / icon->getTextureRect().width,
            rect.height / icon->getTextureRect().height);
    icon->setScale(scaling, scaling);
    icon->setPosition(rect.height / 2, rect.height / 2);
    icon->setFillColor(COL_RED);
    icon->setAlpha(200);
    icon->add(new TweenRotation(0, 360, 2, TweenLoop::Loop));
    add(icon);

    titleText = new Text("Please Wait...", (unsigned int) (getSize().y * 0.66f));
    titleText->setFillColor(COL_RED);
    titleText->setPosition(icon->getSize().x / 2 + 10, 0);
    add(titleText);

    messageText = new Text("Doing something in background, please wait", (unsigned int) (getSize().y * 0.33f));
    messageText->setFillColor(COL_RED);
    messageText->setOrigin(Origin::BottomLeft);
    messageText->setPosition(icon->getSize().x / 2 + 10, getSize().y - 2);
    messageText->setSizeMax(getSize().x - icon->getSize().x - 16, 0);
    add(messageText);

    clock = new C2DClock();
    //mutex = SDL_CreateMutex();

    tween = new TweenAlpha(0, 255, 0.5f);
    add(tween);
    setVisibility(Visibility::Hidden);
}

StatusBox::~StatusBox() {
    delete (clock);
    //SDL_DestroyMutex(mutex);
}

void StatusBox::show(const std::string &title, const std::string &message, bool inf, bool drawNow) {

    //SDL_LockMutex(mutex);
    titleText->setString(title);
    messageText->setString(message);
    //SDL_UnlockMutex(mutex);

    infinite = inf;
    clock->restart();
    setVisibility(Visibility::Visible, true);
    icon->setVisibility(Visibility::Visible, true);
    if (drawNow) {
        for (int i = 0; i < 10; i++) {
            retroDream->getRender()->flip();
        }
    }
}

void StatusBox::hide() {
    clock->restart();
    infinite = false;
}

void StatusBox::onDraw(c2d::Transform &transform, bool draw) {

    if (isVisible() && !infinite && clock->getElapsedTime().asSeconds() > 2) {
        setVisibility(Visibility::Hidden, true);
    }

    //SDL_LockMutex(mutex);
    C2DObject::onDraw(transform, draw);
    //SDL_UnlockMutex(mutex);
}

void StatusBox::setAlpha(uint8_t alpha, bool recursive) {
    C2DObject::setAlpha(alpha, recursive);
}
