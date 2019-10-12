//
// Created by Irwin on 2019-10-11.
//

#include "AnimWebPMaker.h"

int AnimWebPMaker::setLoopCount(int loop_count, WebPData *const webp_data) {
    int ok;
    WebPMuxError err;
    uint32_t features;
    WebPMuxAnimParams new_params;
    WebPMux *const mux = WebPMuxCreate(webp_data, 1);
    if (mux == NULL) return 0;

    err = WebPMuxGetFeatures(mux, &features);
    ok = (err == WEBP_MUX_OK);
    if (!ok || !(features & ANIMATION_FLAG)) goto End;

    err = WebPMuxGetAnimationParams(mux, &new_params);
    ok = (err == WEBP_MUX_OK);
    if (ok) {
        new_params.loop_count = loop_count;
        err = WebPMuxSetAnimationParams(mux, &new_params);
        ok = (err == WEBP_MUX_OK);
    }
    if (ok) {
        WebPDataClear(webp_data);
        err = WebPMuxAssemble(mux, webp_data);
        ok = (err == WEBP_MUX_OK);
    }

    End:
    WebPMuxDelete(mux);
    if (!ok) {
        LOGE("Error during loop-count setting\n");
    }
    return ok;
}

AnimWebPMaker::AnimWebPMaker() {
    enc = NULL;
    pic_num = 0;
    timestamp_ms = 0;
    width = 0;
    height = 0;
}

int AnimWebPMaker::init() {
    WebPDataInit(&webp_data);
    if (!WebPAnimEncoderOptionsInit(&anim_config) ||
        !WebPConfigInit(&config) ||
        !WebPPictureInit(&pic)) {
        LOGE("Library version mismatch!\n");
        return 0;
    }
    return 1;
}

void AnimWebPMaker::setMin(int min) {
    anim_config.kmin = min;
}

void AnimWebPMaker::setMax(int max) {
    anim_config.kmax = max;
}

void AnimWebPMaker::setMinSize(bool minSize) {
    anim_config.minimize_size = minSize ? 1 : 0;
}

void AnimWebPMaker::setMixed(bool mixed) {
    if (mixed) {
        anim_config.allow_mixed = 1;
        config.lossless = 0;
    } else {
        config.lossless = 1;
    }
}

void AnimWebPMaker::setDuration(int duration) {
    this->duration = duration;
}

int AnimWebPMaker::addImage(const uint8_t *data, size_t data_size, int duration, float quality,
                            bool lossless) {
    int ok;
    if (duration <= 0) {
        duration = this->duration;
    }

    if (duration <= 0) {
        LOGE("Invalid negative duration (%d)\n", duration);
        return 0;
    }
    config.quality = quality;
    if (!anim_config.allow_mixed) {
        if (lossless) {
            config.lossless = 1;
        } else {
            config.lossless = 0;
        }
    } else {
        config.lossless = 1;
    }


    ok = WebPValidateConfig(&config);
    if (!ok) {
        LOGE("Invalid configuration.\n");
        return ok;
    }

    //For Windows Image Content, Not Available.
    //#ifdef HAVE_WINCODEC_H
    //  // Try to decode the file using WIC falling back to the other readers for
    //  // e.g., WebP.
    //  ok = ReadPictureWithWIC(filename, pic, 1, NULL);
    //  if (ok) return 1;
    //#endif

    pic.use_argb = 1;
    //Read image.
    WebPImageReader reader = WebPGuessImageReader(data, data_size);
    ok = reader(data, data_size, &pic, 1, NULL);
    if (!ok) {
        LOGE("Read image fail.\n");
        return ok;
    }
    if (enc == NULL) {
        width = pic.width;
        height = pic.height;
        enc = WebPAnimEncoderNew(width, height, &anim_config);
        ok = (enc != NULL);
        if (!ok) {
            LOGE("Could not create WebPAnimEncoder object.\n");
        }
    }

    if (ok) {
        ok = (width == pic.width && height == pic.height);
        if (!ok) {
            LOGE("Frame #%d dimension mismatched! "
                 "Got %d x %d. Was expecting %d x %d.\n",
                 pic_num, pic.width, pic.height, width, height);
        }
    }

    if (ok) {
        ok = WebPAnimEncoderAdd(enc, &pic, timestamp_ms, &config);
        if (!ok) {
            LOGE("Error while adding frame #%d\n", pic_num);
        }
    }
    WebPPictureFree(&pic);
    if (!ok) {
        return ok;
    }

    timestamp_ms += duration;
    ++pic_num;

    return ok;

}

int AnimWebPMaker::make(int loop_count, const char *output) {
    // add a last fake frame to signal the last duration
    int ok = WebPAnimEncoderAdd(enc, NULL, timestamp_ms, NULL);
    ok = ok && WebPAnimEncoderAssemble(enc, &webp_data);
    if (!ok) {
        LOGE("Error during final animation assembly.\n");
    } else {
        ok = setLoopCount(loop_count, &webp_data);
        if (ok) {
            if (output != NULL) {
                ok = ImgIoUtilWriteFile(output, webp_data.bytes, webp_data.size);
                if (ok) WFPRINTF(stderr, "output file: %s     ", (const W_CHAR *) output);
            } else {
                LOGE("[no output file specified]   ");
            }
        }
        if (ok) {
            LOGD("[%d frames, %u bytes].\n",
                 pic_num, (unsigned int) webp_data.size);
        }
    }
    return ok;
}

AnimWebPMaker::~AnimWebPMaker() {
    // free resources
    if (enc != NULL) {
        WebPAnimEncoderDelete(enc);
    }

    WebPDataClear(&webp_data);
}
