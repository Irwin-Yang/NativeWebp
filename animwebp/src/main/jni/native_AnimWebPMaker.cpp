//
// Created by Irwin on 2019-10-11.
//
#define LOG_TAG "AnimWebp"

#include "native_AnimWebPMaker.h"
#include "AnimWebPMaker.h"
#include "logutils.h"

struct fields_t {
    jfieldID context;
};
static fields_t fields;

static AnimWebPMaker *getMaker(JNIEnv *env, jobject thiz) {
    return (AnimWebPMaker *) env->GetLongField(thiz, fields.context);
}

static int ReadImage(const char filename[], WebPPicture *const pic) {
    const uint8_t *data = NULL;
    size_t data_size = 0;
    WebPImageReader reader;
    int ok;
    //For Windows Image Content, Not available
    // #ifdef HAVE_WINCODEC_H
    //   // Try to decode the file using WIC falling back to the other readers for
    //   // e.g., WebP.
    //   ok = ReadPictureWithWIC(filename, pic, 1, NULL);
    //   if (ok) return 1;
    // #endif
    if (!ImgIoUtilReadFile(filename, &data, &data_size)) return 0;
    reader = WebPGuessImageReader(data, data_size);
    ok = reader(data, data_size, pic, 1, NULL);
    free((void *) data);
    return ok;
}

extern "C" JNIEXPORT void JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeInit
        (JNIEnv *env, jclass callClazz) {
    jclass clazz;
    clazz = env->FindClass("com/irwin/animwebp/AnimWebPMaker");
    if (clazz == NULL) {
        return;
    }
    fields.context = env->GetFieldID(clazz, "nativeContext", "J");
    if (fields.context == NULL) {
        env->DeleteLocalRef(clazz);
        return;
    }
    env->DeleteLocalRef(clazz);
}

//nativeMakeOnce(String[] images, boolean mixed, int loopCount, int duration, float quality, String outputPath)
extern "C" JNIEXPORT jint JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeMakeOnce
        (JNIEnv *env, jclass callClazz, jobjectArray picArray, jboolean mixed, jint loopCount,
         jint duration, jfloat quality, jstring joutputPath) {

    const char *output = env->GetStringUTFChars(joutputPath, 0);
    if (output == NULL) {
        return 0;
    }
    int ok;
    WebPAnimEncoder *enc = NULL;
    int pic_num = 0;
    int timestamp_ms = 0;
    int loop_count = loopCount;
    int width = 0, height = 0;
    WebPAnimEncoderOptions anim_config;
    WebPConfig config;
    WebPPicture pic;
    WebPData webp_data;
    jstring picPath;
    const char *cPicPath = NULL;
    int picCount;

    WebPDataInit(&webp_data);
    if (!WebPAnimEncoderOptionsInit(&anim_config) ||
        !WebPConfigInit(&config) ||
        !WebPPictureInit(&pic)) {
        LOGE("Library version mismatch!\n");
        ok = 0;
        goto End;
    }
    ok = 1;
    if (loop_count < 0) {
        LOGE("Invalid non-positive loop-count (%d)\n", loop_count);
    }

    // image-reading pass
    pic_num = 0;
    config.quality = quality;
    if (mixed) {
        anim_config.allow_mixed = 1;
        config.lossless = 0;
    } else {
        config.lossless = 1;
    }

    if (duration <= 0) {
        LOGE("Invalid negative duration (%d)\n", duration);
    }

    if (ok) {
        ok = WebPValidateConfig(&config);
        if (!ok) {
            LOGE("Invalid configuration.\n");
            goto End;
        }
    }

    picCount = env->GetArrayLength(picArray);
    for (int i = 0; ok && i < picCount; i++) {
        picPath = (jstring) env->GetObjectArrayElement(picArray, i);
        cPicPath = env->GetStringUTFChars(picPath, NULL);
        // read next input image
        pic.use_argb = 1;
        ok = ReadImage(cPicPath, &pic);
        if (!ok) goto End;

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
        if (!ok) goto End;

        timestamp_ms += duration;
        ++pic_num;

        env->ReleaseStringUTFChars(picPath, cPicPath);
        cPicPath = NULL;

    }

    // add a last fake frame to signal the last duration
    ok = ok && WebPAnimEncoderAdd(enc, NULL, timestamp_ms, NULL);
    ok = ok && WebPAnimEncoderAssemble(enc, &webp_data);
    if (!ok) {
        LOGE("Error during final animation assembly.\n");
    }

    End:
    // free resources
    WebPAnimEncoderDelete(enc);

    if (ok && loop_count > 0) {  // Re-mux to add loop count.
        ok = AnimWebPMaker::setLoopCount(loop_count, &webp_data);
    }

    if (ok) {
        ok = ImgIoUtilWriteFile(output, webp_data.bytes, webp_data.size);
        if (ok) {
            LOGD("output file: %s     ", output);
        }
    }

    if (ok) {
        LOGD("[%d frames, %u bytes].\n",
             pic_num, (unsigned int) webp_data.size);
    }
    env->ReleaseStringUTFChars(joutputPath, output);
    if (cPicPath != NULL) {
        env->ReleaseStringUTFChars(picPath, cPicPath);
    }
    WebPDataClear(&webp_data);
    return ok;
}

extern "C" JNIEXPORT jint JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeSetup
        (JNIEnv *env, jobject thiz) {
    AnimWebPMaker *maker = new AnimWebPMaker();
    int ok = maker->init();
    // Stow our new C++ AnimWebPMaker in an opaque field in the Java object.
    env->SetLongField(thiz, fields.context, (jlong) maker);
    return ok;
}

extern "C" JNIEXPORT void JNICALL
Java_com_irwin_animwebp_AnimWebPMaker_setParameter(JNIEnv *env, jobject thiz, jstring key_,
                                                   jstring value_) {
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return;
    }
    const char *key = env->GetStringUTFChars(key_, 0);
    const char *value = env->GetStringUTFChars(value_, 0);

    if (!strcmp(key, "min")) {
        maker->setMin(atoi(value));
    } else if (!strcmp(key, "max")) {
        maker->setMax(atoi(value));
    } else if (!strcmp(key, "min_size")) {
        maker->setMinSize(atoi(value));
    } else if (!strcmp(key, "mixed")) {
        maker->setMixed(atoi(value));
    } else if (!strcmp(key, "duration")) {
        maker->setDuration(atoi(value));
    } else {
        LOGE("Unsupported parameter %s %s", key, value);
    }

    env->ReleaseStringUTFChars(key_, key);
    env->ReleaseStringUTFChars(value_, value);
}


//extern "C" JNIEXPORT void JNICALL Java_com_irwin_animwebp_AnimWebPMaker_config
//        (JNIEnv *env, jobject thiz, jint jmin, jint jmax, jboolean jminSize, jboolean jmixed,
//         jint jduration) {
//    AnimWebPMaker *maker = getMaker(env, thiz);
//    if (maker == NULL) {
//        return;
//    }
//    maker->setMin(jmin);
//    maker->setMax(jmax);
//    maker->setMinSize(jminSize);
//    maker->setMixed(jmixed);
//    maker->setDuration(jduration);
//}


extern "C" JNIEXPORT jint JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeAddImage
        (JNIEnv *env, jobject thiz, jbyteArray jarray, jint jsize, jint duration, jfloat quality,
         jboolean lossless) {
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return 0;
    }

    jboolean isCopy;
    jbyte *array = env->GetByteArrayElements(jarray, &isCopy);
    const uint8_t *data = (const uint8_t *) array;
    if (data == NULL) {
        return 0;
    }
    size_t size = (size_t) jsize;
    int ok = maker->addImage(data, size, duration, quality, lossless);
    env->ReleaseByteArrayElements(jarray, array, JNI_ABORT);
    return ok;
}

extern "C" JNIEXPORT jint JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeAddImagePath
        (JNIEnv *env, jobject thiz, jstring img_path, jint duration, jfloat quality,
         jboolean lossless) {
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return 0;
    }
    const char *path = env->GetStringUTFChars(img_path, NULL);
    if (path == NULL) {
        return 0;
    }
    int ok = maker->addImage(path, duration, quality, lossless);
    env->ReleaseStringUTFChars(img_path, path);
    return ok;
}


extern "C" JNIEXPORT jint JNICALL Java_com_irwin_animwebp_AnimWebPMaker_make
        (JNIEnv *env, jobject thiz, jint loopCount, jstring output) {
    const char *path = env->GetStringUTFChars(output, NULL);
    if (path == NULL) {
        return 0;
    }
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return 0;
    }
    int ok = maker->make(loopCount, path);
    env->ReleaseStringUTFChars(output, path);
    return ok;
}

extern "C" JNIEXPORT void JNICALL
Java_com_irwin_animwebp_AnimWebPMaker_nativeReset(JNIEnv *env, jobject thiz) {
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return;
    }
    maker->reset();
}


extern "C" JNIEXPORT void JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeRelease
        (JNIEnv *env, jobject thiz) {
    AnimWebPMaker *maker = getMaker(env, thiz);
    if (maker == NULL) {
        return;
    }
    delete maker;
}

extern "C" JNIEXPORT void JNICALL Java_com_irwin_animwebp_AnimWebPMaker_nativeFinalize
        (JNIEnv *env, jobject thiz) {
    Java_com_irwin_animwebp_AnimWebPMaker_nativeRelease(env, thiz);
}