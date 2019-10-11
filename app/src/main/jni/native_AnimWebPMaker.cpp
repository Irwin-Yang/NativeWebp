//
// Created by Irwin on 2019-10-11.
//
#include "native_AnimWebPMaker.h"
#include "AnimWebPMaker.h"

struct fields_t {
    jfieldID    context;
};
static fields_t fields;

static  AnimWebPMaker* getMaker(JNIEnv *env, jobject thiz)
{
    return (AnimWebPMaker*)env->GetLongField(thiz,fields.context);
}

JNIEXPORT void JNICALL Java_com_irwin_webplib_AnimWebPMaker_nativeInit
  (JNIEnv *env, jobject thiz){
    jclass clazz;
    clazz=env->FindClass("com/irwin/webplib/AnimWebPMaker");
    if(clazz==NULL)
    {
     return ;
    }
    fields.context=env->GetFieldID(clazz,"nativeContext","J");
    if(fields.context==NULL)
    {
    return ;
    }
    env->DeleteLocalRef(clazz);
  }

JNIEXPORT jint JNICALL Java_com_irwin_webplib_AnimWebPMaker_nativeSetup
  (JNIEnv *env, jobject thiz){
       AnimWebPMaker* maker = new AnimWebPMaker();
       if (maker == NULL) {
           return 1;
       }

       // Stow our new C++ MediaPlayer in an opaque field in the Java object.
     env->SetLongField(thiz, fields.context, (jlong)maker);
    return 0;
  }


JNIEXPORT void JNICALL Java_com_irwin_webplib_AnimWebPMaker_config
  (JNIEnv *env, jobject thiz, jint jmin, jint jmax, jboolean jminSize, jboolean jmixed, jint jduration){
       AnimWebPMaker* maker=getMaker(env,thiz);
       if(maker==NULL){
       return ;
       }
       maker->setMin(jmin);
       maker->setMax(jmax);
       maker->setMinSize(jminSize);
       maker->setMixed(jmixed);
       maker->setDuration(jduration);
  }


JNIEXPORT jint JNICALL Java_com_irwin_webplib_AnimWebPMaker_nativeAddImage
  (JNIEnv *env, jobject thiz, jbyteArray array, jint size, jint duration , jfloat quality, jboolean lossless){
        AnimWebPMaker* maker=getMaker(env,thiz);
         if(maker==NULL){
         return 1;
         }

        bool isCopy;
        const uint8_t* data=env->GetByteArrayElements(array,&isCopy);
        if(data==NULL){
        return 1;
        }
        int ok= maker->addImage(data,size,duration,quality,lossless);
        env->ReleaseByteArrayElements(array,data,JNI_ABORT);
        return ok;
  }


JNIEXPORT jint JNICALL Java_com_irwin_webplib_AnimWebPMaker_make
  (JNIEnv *env, jobject thiz, jint loopCount, jstring output){
         const char *path=env->GetStringUTFChars(output,NULL);
         if(path==NULL){
         return 1;
         }
          AnimWebPMaker* maker=getMaker(env,thiz);
          if(maker==NULL){
          return 1;
          }
         int ok= maker->make(loopCount,path);
          env->ReleaseStringUTFChars(output, path);
          return ok;
  }


JNIEXPORT void JNICALL Java_com_irwin_webplib_AnimWebPMaker_nativeRelease
  (JNIEnv *env, jobject thiz){
          AnimWebPMaker* maker=getMaker(env,thiz);
          if(maker==NULL){
          return ;
          }
          delete maker;
  }

JNIEXPORT void JNICALL Java_com_irwin_webplib_AnimWebPMaker_nativeFinalize
  (JNIEnv *env, jobject thiz){
    Java_com_irwin_webplib_AnimWebPMaker_nativeRelease(env,thiz);
  }