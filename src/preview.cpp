//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "preview.h"

using namespace c2d;

static roq_state state;

static int decodeThread(void *data) {

    auto rd = (RetroDream *) data;
    auto preview = rd->getPreview();

    int framerate;
    int chunk_id;
    unsigned int chunk_size;
    unsigned int chunk_arg;
    roq_audio audio;
    int initialized = 0;
    int snd_left, snd_right;
    bool loop = true;
    FILE *file = nullptr;
    size_t file_ret;
    auto file_buffer = (unsigned char *) malloc(MAX_BUF_SIZE);
    C2DClock clock;
    C2DClock timer;

    while (!rd->getPreview()->thread_stop) {
        // load roq file
        if (preview->status == ROQ_LOAD) {
            // open file
            if (file) {
                fclose(file);
            }
            file = fopen(preview->previewPath.c_str(), "rb");
            if (!file) {
                printf("ROQ_FILE_OPEN_FAILURE: %s\n", preview->previewPath.c_str());
                preview->hide(ROQ_FILE_OPEN_FAILURE);
                continue;
            }
            // read roq header
            file_ret = fread(file_buffer, CHUNK_HEADER_SIZE, 1, file);
            if (file_ret != 1) {
                printf("ROQ_FILE_READ_FAILURE: %s\n", preview->previewPath.c_str());
                preview->hide(ROQ_FILE_READ_FAILURE);
                continue;
            }
            // verify signature
            chunk_id = LE_16(&file_buffer[0]);
            chunk_size = LE_32(&file_buffer[2]);
            if (chunk_id != RoQ_SIGNATURE || chunk_size != 0xFFFFFFFF) {
                printf("ROQ_FILE_READ_FAILURE\n");
                preview->hide(ROQ_SIG_FAILURE);
                continue;
            }
            // extract framerate
            framerate = LE_16(&file_buffer[6]);
            printf("RoQ file plays at %d frames/sec\n", framerate);
            // Initialize Audio SQRT Look-Up Table
            for (int i = 0; i < 128; i++) {
                audio.snd_sqr_array[i] = i * i;
                audio.snd_sqr_array[i + 128] = -(i * i);
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
            preview->mutex->unlock();
            continue;
        }

        // sleep thread if not playing
        if (preview->status != ROQ_PLAYING) {
            rd->getRender()->delay(100);
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
                preview->hide(ROQ_STOPPED);
                continue;
            }
        }

        chunk_id = LE_16(&file_buffer[0]);
        chunk_size = LE_32(&file_buffer[2]);
        chunk_arg = LE_16(&file_buffer[6]);

        if (chunk_size > MAX_BUF_SIZE) {
            preview->hide(ROQ_CHUNK_TOO_LARGE);
            continue;
        }

        file_ret = fread(file_buffer, chunk_size, 1, file);
        if (file_ret != 1) {
            preview->hide(ROQ_FILE_READ_FAILURE);
            continue;
        }

        switch (chunk_id) {
            case RoQ_INFO:
                if (initialized)
                    break;

                state.alpha = chunk_arg;
                state.width = LE_16(&file_buffer[0]);
                state.height = LE_16(&file_buffer[2]);
                /* width and height each need to be divisible by 16 */
                if ((state.width & 0xF) || (state.height & 0xF)) {
                    preview->hide(ROQ_INVALID_PIC_SIZE);
                    break;
                }
                state.mb_width = state.width / 16;
                state.mb_height = state.height / 16;
                state.mb_count = state.mb_width * state.mb_height;
                if (state.width < 8 || state.width > 1024) {
                    preview->hide(ROQ_INVALID_DIMENSION);
                } else {
                    state.stride = 8;
                    while (state.stride < state.width)
                        state.stride <<= 1;
                }
                if (state.height < 8 || state.height > 1024) {
                    preview->hide(ROQ_INVALID_DIMENSION);
                } else {
                    state.texture_height = 8;
                    while (state.texture_height < state.height)
                        state.texture_height <<= 1;
                }
                printf("  RoQ_INFO: dimensions = %dx%d, %dx%d; %d mbs, texture = %dx%d\n",
                       state.width, state.height, state.mb_width, state.mb_height,
                       state.mb_count, state.stride, state.texture_height);
                state.frame[0] = (unsigned char *) malloc(
                        state.texture_height * state.stride * sizeof(unsigned short));
                state.frame[1] = (unsigned char *) malloc(
                        state.texture_height * state.stride * sizeof(unsigned short));
                state.current_frame = 0;
                if (!state.frame[0] || !state.frame[1]) {
                    free(state.frame[0]);
                    free(state.frame[1]);
                    preview->hide(ROQ_NO_MEMORY);
                    break;
                }
                memset(state.frame[0], 0, state.texture_height * state.stride * sizeof(unsigned short));
                memset(state.frame[1], 0, state.texture_height * state.stride * sizeof(unsigned short));

                // set this flag so that this code is not executed again when looping
                initialized = 1;
                break;

            case RoQ_QUAD_CODEBOOK:
                preview->status = roq_unpack_quad_codebook_rgb565(file_buffer, chunk_size, chunk_arg, &state);
                break;

            case RoQ_QUAD_VQ:
                if (timer.getElapsedTime().asMilliseconds() > 16 * 60) {
                    printf("roq_unpack: %i ms (frame: %i, fps: %f)\n",
                           clock.getElapsedTime().asMilliseconds(),
                           rd->getRender()->getDeltaTime().asMilliseconds(),
                           rd->getRender()->getFps());
                    timer.restart();
                }

                // frame limit
                while (clock.getElapsedTime().asMilliseconds() < 1000 / framerate) {
                    rd->getRender()->delay(5);
                }
                // update buffers
                preview->mutex->lock();
                clock.restart();
                preview->status = roq_unpack_vq_rgb565(file_buffer, chunk_size, chunk_arg, &state);
                preview->mutex->unlock();

                preview->videoUpload = true;
                state.current_frame++;
                break;

            case RoQ_JPEG:
                break;

            case RoQ_SOUND_MONO:
                audio.channels = 1;
                audio.pcm_samples = chunk_size * 2;
                snd_left = chunk_arg;
                for (unsigned int i = 0; i < chunk_size; i++) {
                    snd_left += audio.snd_sqr_array[file_buffer[i]];
                    audio.pcm_sample[i * 2] = snd_left & 0xff;
                    audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
                }
                /* TODO
                if (cbs->audio_cb)
                    status = cbs->audio_cb(roq_audio.pcm_sample, roq_audio.pcm_samples,
                                           roq_audio.channels);
                */
                break;

            case RoQ_SOUND_STEREO:
                audio.channels = 2;
                audio.pcm_samples = chunk_size * 2;
                snd_left = (chunk_arg & 0xFF00);
                snd_right = (chunk_arg & 0xFF) << 8;
                for (unsigned int i = 0; i < chunk_size; i += 2) {
                    snd_left += audio.snd_sqr_array[file_buffer[i]];
                    snd_right += audio.snd_sqr_array[file_buffer[i + 1]];
                    audio.pcm_sample[i * 2] = snd_left & 0xff;
                    audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
                    audio.pcm_sample[i * 2 + 2] = snd_right & 0xff;
                    audio.pcm_sample[i * 2 + 3] = (snd_right & 0xff00) >> 8;
                }
                /* TODO
                if (cbs->audio_cb)
                    status = cbs->audio_cb(roq_audio.pcm_sample, roq_audio.pcm_samples,
                                           roq_audio.channels);
                */
                break;

            case RoQ_PACKET:
                /* still unimplemented */
                break;

            default:
                break;
        }
    }

    if (file) {
        fclose(file);
    }
    if (state.frame[0]) {
        free(state.frame[0]);
        state.frame[0] = nullptr;
    }
    if (state.frame[1]) {
        free(state.frame[1]);
        state.frame[1] = nullptr;
    }
    if (file_buffer != nullptr) {
        free(file_buffer);
    }

    return 0;
}

Preview::Preview(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    retroDream = rd;

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.1f));
    setVisibility(Visibility::Hidden);
}

void Preview::hide(int i) {
    mutex->lock();
    status = i;
    setVisibility(Visibility::Hidden, true);
    mutex->unlock();
}

bool Preview::load(const std::string &path) {

    unload();

    // set loaded to true so we don't loop on non-existing preview
    loaded = true;

    // roq video
    if (Utility::endsWith(path, ".roq")) {
        if (!mutex) {
            mutex = new C2DMutex();
        }
        if (!thread) {
            thread = new C2DThread(decodeThread, retroDream);
        }
        previewPath = path;
        status = ROQ_LOAD;
    } else {
        // preview image
        texture = new C2DTexture(path);
        if (!texture->available) {
            delete (texture);
            texture = nullptr;
            return false;
        }

        texture->setOrigin(Origin::Left);
        texture->setPosition(Vector2f(8, getSize().y / 2));
        texture_scaling = std::min(
                getSize().x / ((float) texture->getTextureRect().width + 16),
                getSize().y / ((float) texture->getTextureRect().height + 16));
        texture->setScale(texture_scaling, texture_scaling);
        add(texture);
        setVisibility(Visibility::Visible, true);
    }

    return true;
}

void Preview::unload() {

    // roq video player
    if (videoTex != nullptr) {
        delete (videoTex);
        videoTex = nullptr;
    }
    status = ROQ_STOPPED;

    if (texture != nullptr) {
        delete (texture);
        texture = nullptr;
    }

    if (isVisible()) {
        setVisibility(Visibility::Hidden, true);
    }

    loaded = false;
}

bool Preview::isLoaded() {
    return loaded;
}

void Preview::onUpdate() {

    if (status == ROQ_PLAYING && videoUpload) {
        if (!videoTex) {
            videoTex = new C2DTexture({state.width, state.width}, Texture::Format::RGB565);
            videoTex->setOrigin(Origin::Left);
            videoTex->setPosition(Vector2f(8, getSize().y / 2));
            texture_scaling = std::min(
                    getSize().x / ((float) videoTex->getTextureRect().width + 32),
                    getSize().y / ((float) videoTex->getTextureRect().height + 32));
            videoTex->setScale(texture_scaling, texture_scaling);
            add(videoTex);
        }

        mutex->lock();
        //C2DClock clock;
        videoTex->unlock(state.frame[state.current_frame & 1]);
        //printf("videoTex->unlock: %i ms (fps: %f)\n",
        //     clock.restart().asMilliseconds(), retroDream->getRender()->getFps());
        mutex->unlock();
        videoUpload = false;
    }

    RoundedRectangleShape::onUpdate();
}

Preview::~Preview() {
    thread_stop = true;
    thread->join();
    delete (thread);
    delete (mutex);
}
