//
// Created by cpasjuste on 02/02/2021.
//

#ifndef RETRODREAM_ROQPLAYER_H
#define RETRODREAM_ROQPLAYER_H

#include <cross2d/c2d.h>
#include "roqlib.h"

class RoqPlayerTexture : public c2d::C2DTexture {

public:
    RoqPlayerTexture();

    int play(const std::string &path);

    void stop();

private:
    void onUpdate() override;

    FILE *file;
    size_t file_ret;
    int framerate;
    int chunk_id;
    unsigned int chunk_size;
    unsigned int chunk_arg;
    roq_state state;
    roq_audio audio;
    int status = ROQ_STOPPED;
    int initialized = 0;
    unsigned char read_buffer[MAX_BUF_SIZE];
    int snd_left, snd_right;
    bool loop = true;
    int colorspace = ROQ_RGB565;
};

#endif //RETRODREAM_ROQPLAYER_H
