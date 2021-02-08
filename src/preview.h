//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEW_H
#define RETRODREAM_PREVIEW_H

#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"
#include "roqlib.h"

// preview video 128
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=128:-2" -t 30 video.roq
// preview video 256
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=256:-2" -t 30 video.roq

class RetroDream;

class Preview : public c2d::RoundedRectangleShape {

public:

    explicit Preview(RetroDream *rd, const c2d::FloatRect &rect);

    ~Preview() override;

    bool load(const std::string &path);

    void unload();

    bool isImageLoaded() const;

    bool isVideoLoaded() const;

    void hide(int status);

    std::string previewPath;
    c2d::Mutex *mutex = nullptr;
    bool thread_stop = false;
    int status = ROQ_STOPPED;
    bool videoUpload = false;
    c2d::Audio *audio = nullptr;

private:

    void onUpdate() override;

    RetroDream *retroDream = nullptr;
    bool loaded = false;
    c2d::Thread *thread = nullptr;
    c2d::Texture *texture = nullptr;
    c2d::Sprite *sprite = nullptr;
    float texture_scaling;
};

#endif //RETRODREAM_PREVIEW_H
