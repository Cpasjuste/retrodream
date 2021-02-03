//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEW_H
#define RETRODREAM_PREVIEW_H

#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"
#include "roqlib.h"

class Preview : public c2d::RoundedRectangleShape {

public:

    Preview(const c2d::FloatRect &rect);

    bool load(const std::string &path);

    void unload();

    bool isLoaded();

private:

    c2d::Texture *texture = nullptr;
    float texture_scaling;
    bool loaded = false;

    // roq video player
    // ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=256:-2" -t 30 video.roq
    void onUpdate() override;

    FILE *file = nullptr;
    size_t file_ret = 0;
    unsigned char *file_buffer = nullptr;
    int framerate;
    int chunk_id;
    unsigned int chunk_size;
    unsigned int chunk_arg;
    roq_state state;
    roq_audio audio;
    int status = ROQ_STOPPED;
    int initialized = 0;
    int snd_left, snd_right;
    bool loop = true;
    int colorspace = ROQ_RGB565;

};

#endif //RETRODREAM_PREVIEW_H
