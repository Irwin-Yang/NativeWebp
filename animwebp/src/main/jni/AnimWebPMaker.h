//
// Created by Irwin on 2019-10-11.
//
#define LOG_TAG "AnimWebp"
#ifndef WEBPLIB_ANIMWEBPMAKER_H
#define WEBPLIB_ANIMWEBPMAKER_H

#include "imageio/image_dec.h"
#include "imageio/imageio_util.h"
#include "./stopwatch.h"
#include "./unicode.h"
#include "libwebp/webp/encode.h"
#include "libwebp/webp/mux.h"
#include "logutils.h"

class AnimWebPMaker {

private:
     WebPAnimEncoder* enc;
     int duration;
     WebPAnimEncoderOptions anim_config;
     WebPConfig config;
     WebPPicture pic;
     WebPData webp_data;
     int pic_num ;
     int timestamp_ms;
     int width;
     int height;
     int checkDuration(int *duration);

public:
    AnimWebPMaker();
    static int setLoopCount(int loop_count, WebPData* const webp_data);
    int init();
    void setMin(int min);
    void setMax(int max);
    void setMinSize(bool minSize);
    void setMixed(bool mixed);
    void setDuration(int duration);
    int addImage(const uint8_t* data,size_t data_size,int duration,float quality,bool lossless);
    int addImage(const char* file_path,int duration,float quality,bool lossless);
    int make(int loop_count,const char* output);
    void reset();
    ~AnimWebPMaker();
};



#endif //WEBPLIB_ANIMWEBPMAKER_H
