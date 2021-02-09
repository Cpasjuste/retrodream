//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEWVIDEO_H
#define RETRODREAM_PREVIEWVIDEO_H

#include "cross2d/skeleton/sfml/RoundedRectangleShape.h"
#include "roqlib.h"

// preview video 128
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=128:-2" -t 30 video.roq
// preview video 256
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=256:-2" -t 30 video.roq

class RetroDream;

class PreviewVideo : public c2d::RoundedRectangleShape {

public:

    explicit PreviewVideo(RetroDream *rd, RetroConfig::CustomShape *shape);

    ~PreviewVideo() override;

    bool load(const std::string &path);

    void unload();

    bool isLoaded() const;

    void hide(int status);

    RetroDream *retroDream = nullptr;
    std::string previewPath;
    c2d::Mutex *mutex = nullptr;
    bool thread_stop = false;
    int status = ROQ_STOPPED;
    bool videoUpload = false;
    c2d::Audio *audio = nullptr;

private:

    void onUpdate() override;

    bool loaded = false;
    c2d::Thread *thread = nullptr;
    c2d::Texture *texture = nullptr;
    c2d::Sprite *sprite = nullptr;
    float texture_scaling;
};

#endif //RETRODREAM_PREVIEWVIDEO_H
