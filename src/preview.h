//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEW_H
#define RETRODREAM_PREVIEW_H

#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"
#include "roqlib.h"

// preview video with audio (roq)
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=256:-2" -t 30 video.roq

// preview video without audio (roq)
// ffmpeg -i video.mp4 -an -framerate 15 -vf "scale=256:-2" -t 30 video.roq

class RetroDream;

class Preview : public c2d::RoundedRectangleShape {

public:

    explicit Preview(RetroDream *rd, const c2d::FloatRect &rect);

    ~Preview() override;

    bool load(const std::string &path);

    void unload();

    bool isLoaded();

    void hide(int status);

    std::string previewPath;
    c2d::Mutex *mutex = nullptr;
    bool thread_stop = false;
    int status = ROQ_STOPPED;
    // preview video
    c2d::Texture *videoTex = nullptr;
    bool videoUpload = false;
    float texture_scaling;

private:

    void onUpdate() override;

    RetroDream *retroDream = nullptr;
    bool loaded = false;
    c2d::Thread *thread = nullptr;
    // preview image
    c2d::Texture *texture = nullptr;
};

#endif //RETRODREAM_PREVIEW_H
