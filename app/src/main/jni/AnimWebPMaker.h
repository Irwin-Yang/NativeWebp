//
// Created by Irwin on 2019-10-11.
//

#ifndef WEBPLIB_ANIMWEBPMAKER_H
#define WEBPLIB_ANIMWEBPMAKER_H

#include "imageio/image_dec.h"
#include "imageio/imageio_util.h"
#include "./stopwatch.h"
#include "./unicode.h"
#include "libwebp/webp/encode.h"
#include "libwebp/webp/mux.h"

class AnimWebPMaker {

private:
     WebPAnimEncoder* enc = NULL;
     int duration;
     WebPAnimEncoderOptions anim_config;
     WebPConfig config;
     WebPPicture pic;
     WebPData webp_data;
     int pic_num = 0;
     int timestamp_ms = 0;

public:
    static int setLoopCount(int loop_count, WebPData* const webp_data);
    int init();
    void setMin(int min);
    void setMax(int max);
    void setMinSize(bool minSize);
    void setMixed(bool mixed);
    void setDuration(int duration);
    int addImage(const uint8_t* data,size_t size,int duration,float quality,bool lossless);
    int make(int loopCount,const char* output);
    ~AnimWebPMaker();
};



#endif //WEBPLIB_ANIMWEBPMAKER_H
