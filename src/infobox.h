//
// Created by cpasjuste on 24/01/2020.
//

#ifndef RETRODREAM_INFOBOX_H
#define RETRODREAM_INFOBOX_H

class InfoBox : public c2d::RoundedRectangleShape {

public:

    explicit InfoBox(const c2d::FloatRect &rect);

    c2d::Text *text = nullptr;

};

#endif //RETRODREAM_INFOBOX_H
