//
// Created by cpasjuste on 16/01/2020.
//

#ifndef RETRODREAM_PREVIEWVIDEO_H
#define RETRODREAM_PREVIEWVIDEO_H

#include "skinrect.h"
#include "roqlib.h"

// preview video
// ffmpeg -i video.mp4 -ar 22050 -framerate 30 -vf "scale=256:-2" -t 30 video.roq

class RetroDream;

class PreviewVideo : public SkinRect {

public:

    explicit PreviewVideo(RetroDream *rd, Skin::CustomShape *shape);

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
};

#endif //RETRODREAM_PREVIEWVIDEO_H
