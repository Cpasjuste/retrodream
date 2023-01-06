//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "previewvideo.h"

using namespace c2d;

static roq_state state;

static int decodeThread(void *data) {
    auto rd = (RetroDream *) data;
    auto preview = rd->getPreviewVideo();

    int framerate = 30;
    int chunk_id, res;
    unsigned int chunk_size;
    unsigned int chunk_arg;
    roq_audio roq_audio;
    int initialized = 0;
    int snd_left, snd_right;
    bool loop = true;
    FILE *file = nullptr;
    size_t file_ret;
    C2DClock clock;
    C2DClock timer;

    auto file_buffer = (unsigned char *) malloc(MAX_BUF_SIZE);
    roq_audio.pcm_sample = (unsigned char *) malloc(MAX_BUF_SIZE);

    // be sure preview is available
    while (!preview) {
        rd->getRender()->delay(16);
        preview = rd->getPreviewVideo();
    }

    while (!preview->thread_stop) {

        // load roq file
        if (preview->status == ROQ_LOAD) {
            printf("Preview::Thread: ROQ_LOAD (%s)\n", preview->previewPath.c_str());
            // open file
            if (file) {
                fclose(file);
            }
            file = fopen(preview->previewPath.c_str(), "rb");
            if (!file) {
                printf("ROQ_FILE_OPEN_FAILURE: %s\n", preview->previewPath.c_str());
                preview->hide();
                continue;
            }
            // read roq header
            file_ret = fread(file_buffer, CHUNK_HEADER_SIZE, 1, file);
            if (file_ret != 1) {
                printf("ROQ_FILE_READ_FAILURE: %s\n", preview->previewPath.c_str());
                preview->hide();
                continue;
            }
            // verify signature
            chunk_id = LE_16(&file_buffer[0]);
            chunk_size = LE_32(&file_buffer[2]);
            if (chunk_id != RoQ_SIGNATURE || chunk_size != 0xFFFFFFFF) {
                printf("ROQ_FILE_READ_FAILURE\n");
                preview->hide();
                continue;
            }
            // extract framerate
            framerate = LE_16(&file_buffer[6]);
            printf("RoQ file plays at %d frames/sec\n", framerate);
            // Initialize Audio SQRT Look-Up Table
            for (int i = 0; i < 128; i++) {
                roq_audio.snd_sqr_array[i] = (short) (i * i);
                roq_audio.snd_sqr_array[i + 128] = (short) -(i * i);
            }

            if (state.frame[0]) {
                free(state.frame[0]);
            }
            if (state.frame[1]) {
                free(state.frame[1]);
            }

            // all done
            initialized = 0;
            preview->mutex->lock();
            preview->status = ROQ_PLAYING;
            preview->setVisibility(Visibility::Visible, true);
            preview->retroDream->getHelpBox()->setVisibility(Visibility::Hidden, true);
            preview->mutex->unlock();
            continue;
        }

        // sleep thread if not playing
        if (preview->status != ROQ_PLAYING) {
            if (preview->status == ROQ_STOPPING) {
                if (file) {
                    fclose(file);
                    file = nullptr;
                }
            }
            preview->status = ROQ_STOPPED;
            rd->getRender()->delay(2);
            continue;
        }

        file_ret = fread(file_buffer, CHUNK_HEADER_SIZE, 1, file);
        if (file_ret != 1) {
            /* if the read failed but the file is not EOF, there is a deeper
             * problem; don't entertain the idea of looping */
            if (!feof(file)) {
                continue;
            } else if (loop) {
                fseek(file, 8, SEEK_SET);
                continue;
            } else {
                preview->hide();
                continue;
            }
        }

        chunk_id = LE_16(&file_buffer[0]);
        chunk_size = LE_32(&file_buffer[2]);
        chunk_arg = LE_16(&file_buffer[6]);

        if (chunk_size > MAX_BUF_SIZE) {
            preview->hide();
            continue;
        }

        file_ret = fread(file_buffer, chunk_size, 1, file);
        if (file_ret != 1) {
            preview->hide();
            continue;
        }

        switch (chunk_id) {
            case RoQ_INFO:
                if (initialized)
                    break;

                state.alpha = (int) chunk_arg;
                state.width = LE_16(&file_buffer[0]);
                state.height = LE_16(&file_buffer[2]);
                /* width and height each need to be divisible by 16 */
                if ((state.width & 0xF) || (state.height & 0xF)) {
                    preview->hide();
                    break;
                }
                state.mb_width = state.width / 16;
                state.mb_height = state.height / 16;
                state.mb_count = state.mb_width * state.mb_height;
                if (state.width < 8 || state.width > 1024) {
                    preview->hide();
                } else {
                    state.stride = 8;
                    while (state.stride < state.width)
                        state.stride <<= 1;
                }
                if (state.height < 8 || state.height > 1024) {
                    preview->hide();
                } else {
                    state.texture_height = 8;
                    while (state.texture_height < state.height)
                        state.texture_height <<= 1;
                }
                printf("RoQ_INFO: dimensions = %dx%d, %dx%d; %d mbs, texture = %dx%d\n",
                       state.width, state.height, state.mb_width, state.mb_height,
                       state.mb_count, state.stride, state.texture_height);
                state.frame[0] = (unsigned char *) malloc(state.width * state.width * 2);
                state.frame[1] = (unsigned char *) malloc(state.width * state.width * 2);
                state.current_frame = 0;
                if (!state.frame[0] || !state.frame[1]) {
                    free(state.frame[0]);
                    free(state.frame[1]);
                    preview->hide();
                    break;
                }
                memset(state.frame[0], 0, state.texture_height * state.stride * sizeof(unsigned short));
                memset(state.frame[1], 0, state.texture_height * state.stride * sizeof(unsigned short));

                // set this flag so that this code is not executed again when looping
                initialized = 1;
                break;

            case RoQ_QUAD_CODEBOOK:
                res = roq_unpack_quad_codebook_rgb565(file_buffer, (int) chunk_size, (int) chunk_arg, &state);
                if (res == ROQ_STOPPED) {
                    preview->status = ROQ_STOPPED;
                }
                break;

            case RoQ_QUAD_VQ:
                // frame limit
                while (clock.getElapsedTime().asMilliseconds() < (1000 / framerate)) {
                    if (preview->thread_stop || preview->status != ROQ_PLAYING) {
                        break;
                    }
#ifdef __DREAMCAST__
                    thd_pass();
#else
                    rd->getRender()->delay(1);
#endif
                }
                clock.restart();

                if (preview->thread_stop || preview->status != ROQ_PLAYING) {
                    break;
                }

                // update buffers
                preview->mutex->lock();
                res = roq_unpack_vq_rgb565(file_buffer, (int) chunk_size, chunk_arg, &state);
                if (res == ROQ_STOPPED) {
                    preview->status = ROQ_STOPPED;
                }
                preview->mutex->unlock();
                if (res == ROQ_PLAYING) {
                    preview->videoUpload = true;
                }
                state.current_frame++;
                break;

            case RoQ_JPEG:
                break;

            case RoQ_SOUND_MONO:
                //printf("RoQ_SOUND_MONO\n");
                if (preview->thread_stop || preview->status != ROQ_PLAYING) {
                    break;
                }
                roq_audio.channels = 1;
                roq_audio.pcm_samples = (int) chunk_size * 2;
                snd_left = (int) chunk_arg;
                for (unsigned int i = 0; i < chunk_size; i++) {
                    snd_left += roq_audio.snd_sqr_array[file_buffer[i]];
                    roq_audio.pcm_sample[i * 2] = snd_left & 0xff;
                    roq_audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
                }
                // TODO: audio
                break;

            case RoQ_SOUND_STEREO:
                if (preview->thread_stop || preview->status != ROQ_PLAYING) {
                    break;
                }
                if ((int) chunk_size * 2 > preview->audio->getSampleBufferAvailable()) {
                    printf("RoQ_SOUND_STEREO: buffer size (%i), audio size (%i) \n",
                           chunk_size * 2, preview->audio->getSampleBufferAvailable());
                    break;
                }
                roq_audio.channels = 2;
                roq_audio.pcm_samples = (int) chunk_size * 2;
                snd_left = (int) (chunk_arg & 0xFF00);
                snd_right = (int) (chunk_arg & 0xFF) << 8;
                for (unsigned int i = 0; i < chunk_size; i += 2) {
                    snd_left += roq_audio.snd_sqr_array[file_buffer[i]];
                    snd_right += roq_audio.snd_sqr_array[file_buffer[i + 1]];
                    roq_audio.pcm_sample[i * 2] = snd_left & 0xff;
                    roq_audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
                    roq_audio.pcm_sample[i * 2 + 2] = snd_right & 0xff;
                    roq_audio.pcm_sample[i * 2 + 3] = (snd_right & 0xff00) >> 8;
                }

                preview->audio->play(roq_audio.pcm_sample,
                                     roq_audio.pcm_samples / roq_audio.channels / (int) sizeof(int16_t));
                break;

            case RoQ_PACKET:
                /* still unimplemented */
            default:
                break;
        }
    }

    if (file) {
        fclose(file);
    }
    if (state.frame[0]) {
        free(state.frame[0]);
    }
    if (state.frame[1]) {
        free(state.frame[1]);
    }
    if (roq_audio.pcm_sample) {
        free(roq_audio.pcm_sample);
    }
    if (file_buffer != nullptr) {
        free(file_buffer);
    }

    return 0;
}

PreviewVideo::PreviewVideo(RetroDream *rd, Skin::CustomShape *shape) : SkinRect(shape) {
    retroDream = rd;
    audio = new C2DAudio(22050, 2048);
    mutex = new C2DMutex();
    thread = new C2DThread(decodeThread, retroDream);

    RectangleShape::setFillColor(Color::Transparent);
    RectangleShape::setVisibility(Visibility::Hidden);
}

void PreviewVideo::hide() {
    mutex->lock();
    status = ROQ_STOPPED;
    setVisibility(Visibility::Hidden);
    retroDream->getHelpBox()->setVisibility(Visibility::Visible, true);
    mutex->unlock();
}

bool PreviewVideo::load(const std::string &path) {
    /*
    printf("PreviewVideo::load: %s\n", path.c_str());

    unload();
    previewPath = path;
    status = ROQ_LOAD;
    loaded = true;
    */
    return true;
}

void PreviewVideo::unload() {
    if (loaded) {
        status = ROQ_STOPPING;
        while (status != ROQ_STOPPED) {
            retroDream->getRender()->delay(2);
        }
        audio->pause(1);
        videoUpload = false;
        if (isVisible()) {
            setVisibility(Visibility::Hidden, true);
            retroDream->getHelpBox()->setVisibility(Visibility::Visible, true);
        }
        loaded = false;
    }
}

bool PreviewVideo::isLoaded() const {
    return loaded;
}

#ifndef NDEBUG
static C2DClock debugClock;
static int unlockTime = 0;
#endif

void PreviewVideo::onUpdate() {
    if (isVisible() && status == ROQ_PLAYING) {
#ifdef __DREAMCAST__
        // frame limit
        while (fpsLimitClock.getElapsedTime().asMilliseconds() < (1000 / 30)) {
            retroDream->getRender()->delay(1);
        }
        fpsLimitClock.restart();
#endif

        if (videoUpload) {
            // TODO: check if width changed
            if (!texture) {
                texture = new C2DTexture({state.width, state.width}, Texture::Format::RGB565);
                texture->setFilter(Texture::Filter::Point);
                texture->setCornerPointCount(CORNER_POINTS);
                texture->setCornersRadius(getCornersRadius());
                texture->setTextureRect(IntRect{0, 0, state.width, state.height});
                texture->setOrigin(Origin::Center);
                texture->setPosition(Vector2f(getSize().x / 2, getSize().y / 2));
                texture->setSize(getSize());
                add(texture);
            }

            mutex->lock();
#ifndef NDEBUG
            if (debugClock.getElapsedTime().asMilliseconds() > 16 * 60) {
                unlockTime = debugClock.restart().asMilliseconds();
            }
#endif
            // TODO: upload frame to pixels buffer directly
            uint8_t *data;
            texture->lock(&data);
            memcpy(data, state.frame[state.current_frame & 1], state.width * state.height * 2);
            texture->unlock();
            //texture->unlock(state.frame[state.current_frame & 1]);
#ifndef NDEBUG
            if (unlockTime > 0) {
                printf("videoTex->unlock: %i ms (fps: %f)\n",
                       debugClock.getElapsedTime().asMilliseconds(), retroDream->getRender()->getFps());
                unlockTime = 0;
            }
#endif
            mutex->unlock();
            videoUpload = false;
        }
    }

    RectangleShape::onUpdate();
}

PreviewVideo::~PreviewVideo() {
    status = ROQ_STOPPED;
    thread_stop = true;
    if (thread) {
        thread->join();
        delete (thread);
    }

    delete (mutex);

    if (audio) {
        delete (audio);
    }
}
