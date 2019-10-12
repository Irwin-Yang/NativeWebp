//
// Created by Irwin on 2019-10-10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "libwebp/webp/config.h"
#endif

#include <jni.h>
#include "imageio/image_dec.h"
#include "imageio/imageio_util.h"
#include "./stopwatch.h"
#include "./unicode.h"
#include "libwebp/webp/encode.h"
#include "libwebp/webp/mux.h"

 static int ReadImage(const char filename[], WebPPicture* const pic) {
    const uint8_t* data = NULL;
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
    free((void*)data);
    return ok;
  }

  static int SetLoopCount(int loop_count, WebPData* const webp_data) {
    int ok = 1;
    WebPMuxError err;
    uint32_t features;
    WebPMuxAnimParams new_params;
    WebPMux* const mux = WebPMuxCreate(webp_data, 1);
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
      fprintf(stderr, "Error during loop-count setting\n");
    }
    return ok;
  }

  //------------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_com_irwin_webplib_WebpProcessor_compose
  (JNIEnv *env, jobject obj, jobjectArray imageArray,jboolean mixed, jint loopCount, jint jduration, jfloat jquality, jstring outputPath)
  {
    const char* output =  (*env)->GetStringUTFChars(env,outputPath,NULL);
    WebPAnimEncoder* enc = NULL;
    int verbose = 0;
    int pic_num = 0;
    int duration = jduration;
    int timestamp_ms = 0;
    int loop_count = loopCount;
    int width = 0, height = 0;
    WebPAnimEncoderOptions anim_config;
    WebPConfig config;
    WebPPicture pic;
    WebPData webp_data;
    int ok;
    jstring path;
    const char* cpath=NULL;

    WebPDataInit(&webp_data);
    if (!WebPAnimEncoderOptionsInit(&anim_config) ||
        !WebPConfigInit(&config) ||
        !WebPPictureInit(&pic)) {
      fprintf(stderr, "Library version mismatch!\n");
      ok = 0;
      goto End;
    }
     ok=1;
     if (loop_count < 0) {
          fprintf(stderr, "Invalid non-positive loop-count (%d)\n", loop_count);
        }

    // image-reading pass
    pic_num = 0;
    config.quality=jquality;
    if(mixed)
    {
        anim_config.allow_mixed = 1;
        config.lossless = 0;
    }
    else{
        config.lossless = 1;
    }

    if (duration <= 0) {
        fprintf(stderr, "Invalid negative duration (%d)\n", duration);
      }

    if (ok) {
        ok = WebPValidateConfig(&config);
        if (!ok) {
          fprintf(stderr, "Invalid configuration.\n");
          goto End;
        }
      }

     int imgCount=(*env)->GetArrayLength(env,imageArray);

     for(int i=0;ok&&i<imgCount;i++)
     {
         path= (jstring)(*env)->GetObjectArrayElement(env,imageArray,i);
         cpath= (*env)->GetStringUTFChars(env,path,NULL);
         // read next input image
         pic.use_argb = 1;
         ok = ReadImage((const char*)cpath, &pic);
         if (!ok) goto End;

         if (enc == NULL) {
           width  = pic.width;
           height = pic.height;
           enc = WebPAnimEncoderNew(width, height, &anim_config);
           ok = (enc != NULL);
           if (!ok) {
             fprintf(stderr, "Could not create WebPAnimEncoder object.\n");
           }
         }

         if (ok) {
           ok = (width == pic.width && height == pic.height);
           if (!ok) {
             fprintf(stderr, "Frame #%d dimension mismatched! "
                             "Got %d x %d. Was expecting %d x %d.\n",
                     pic_num, pic.width, pic.height, width, height);
           }
         }

         if (ok) {
           ok = WebPAnimEncoderAdd(enc, &pic, timestamp_ms, &config);
           if (!ok) {
             fprintf(stderr, "Error while adding frame #%d\n", pic_num);
           }
         }
         WebPPictureFree(&pic);
         if (!ok) goto End;

         if (verbose) {
           WFPRINTF(stderr, "Added frame #%3d at time %4d (file: %s)\n",
                    pic_num, timestamp_ms, cpath);
         }
         timestamp_ms += duration;
         ++pic_num;

         (*env)->ReleaseStringUTFChars(env,path,cpath);
         cpath=NULL;

    }

    // add a last fake frame to signal the last duration
    ok = ok && WebPAnimEncoderAdd(enc, NULL, timestamp_ms, NULL);
    ok = ok && WebPAnimEncoderAssemble(enc, &webp_data);
    if (!ok) {
      fprintf(stderr, "Error during final animation assembly.\n");
    }

   End:
    // free resources
    WebPAnimEncoderDelete(enc);

    if (ok && loop_count > 0) {  // Re-mux to add loop count.
      ok = SetLoopCount(loop_count, &webp_data);
    }

    if (ok) {
      if (output != NULL) {
        ok = ImgIoUtilWriteFile(output, webp_data.bytes, webp_data.size);
        if (ok) WFPRINTF(stderr, "output file: %s     ", (const W_CHAR*)output);
      } else {
        fprintf(stderr, "[no output file specified]   ");
      }
    }

    if (ok) {
      fprintf(stderr, "[%d frames, %u bytes].\n",
              pic_num, (unsigned int)webp_data.size);
    }
    (*env)->ReleaseStringUTFChars(env,outputPath,output);
    if(cpath!=NULL)
    {
     (*env)->ReleaseStringUTFChars(env,path,cpath);
    }
    WebPDataClear(&webp_data);
    FREE_WARGV_AND_RETURN(ok ? (jint)0 : (jint)1);
  }

