//
// Created by cpasjuste on 23/02/2021.
//

#ifndef RETRODREAM_GDPLAY_H
#define RETRODREAM_GDPLAY_H

class GDPlay {
public:

    static bool check(RetroDream *rd);

    static bool run(RetroDream *rd);

    static bool isLoaded();

    static bool isGame();
};


#endif //RETRODREAM_GDPLAY_H
