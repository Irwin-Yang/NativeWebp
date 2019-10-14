LOCAL_PATH := $(call my-dir)
PATH_PREFIX := ./

WEBP_CFLAGS := -Wall -DANDROID -DHAVE_MALLOC_H -DHAVE_PTHREAD -DWEBP_USE_THREAD
WEBP_CFLAGS += -fvisibility=hidden

ifeq ($(APP_OPTIM),release)
  WEBP_CFLAGS += -finline-functions -ffast-math \
                 -ffunction-sections -fdata-sections
  ifeq ($(findstring clang,$(NDK_TOOLCHAIN_VERSION)),)
    WEBP_CFLAGS += -frename-registers -s
  endif
endif

# mips32 fails to build with clang from r14b
# https://bugs.chromium.org/p/webp/issues/detail?id=343
ifeq ($(findstring clang,$(NDK_TOOLCHAIN_VERSION)),clang)
  ifeq ($(TARGET_ARCH),mips)
    clang_version := $(shell $(TARGET_CC) --version)
    ifneq ($(findstring clang version 3,$(clang_version)),)
      WEBP_CFLAGS += -no-integrated-as
    endif
  endif
endif

ifneq ($(findstring armeabi-v7a, $(TARGET_ARCH_ABI)),)
  # Setting LOCAL_ARM_NEON will enable -mfpu=neon which may cause illegal
  # instructions to be generated for armv7a code. Instead target the neon code
  # specifically.
  NEON := c.neon
  USE_CPUFEATURES := yes
  WEBP_CFLAGS += -DHAVE_CPU_FEATURES_H
  $(info Use NEON)
else
  NEON := c
  $(info Use No NEON)
endif

dec_srcs := \
    $(PATH_PREFIX)dec/alpha_dec.c \
    $(PATH_PREFIX)dec/buffer_dec.c \
    $(PATH_PREFIX)dec/frame_dec.c \
    $(PATH_PREFIX)dec/idec_dec.c \
    $(PATH_PREFIX)dec/io_dec.c \
    $(PATH_PREFIX)dec/quant_dec.c \
    $(PATH_PREFIX)dec/tree_dec.c \
    $(PATH_PREFIX)dec/vp8_dec.c \
    $(PATH_PREFIX)dec/vp8l_dec.c \
    $(PATH_PREFIX)dec/webp_dec.c \

demux_srcs := \
    $(PATH_PREFIX)demux/anim_decode.c \
    $(PATH_PREFIX)demux/demux.c \

dsp_dec_srcs := \
    $(PATH_PREFIX)dsp/alpha_processing.c \
    $(PATH_PREFIX)dsp/alpha_processing_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/alpha_processing_neon.$(NEON) \
    $(PATH_PREFIX)dsp/alpha_processing_sse2.c \
    $(PATH_PREFIX)dsp/alpha_processing_sse41.c \
    $(PATH_PREFIX)dsp/cpu.c \
    $(PATH_PREFIX)dsp/dec.c \
    $(PATH_PREFIX)dsp/dec_clip_tables.c \
    $(PATH_PREFIX)dsp/dec_mips32.c \
    $(PATH_PREFIX)dsp/dec_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/dec_msa.c \
    $(PATH_PREFIX)dsp/dec_neon.$(NEON) \
    $(PATH_PREFIX)dsp/dec_sse2.c \
    $(PATH_PREFIX)dsp/dec_sse41.c \
    $(PATH_PREFIX)dsp/filters.c \
    $(PATH_PREFIX)dsp/filters_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/filters_msa.c \
    $(PATH_PREFIX)dsp/filters_neon.$(NEON) \
    $(PATH_PREFIX)dsp/filters_sse2.c \
    $(PATH_PREFIX)dsp/lossless.c \
    $(PATH_PREFIX)dsp/lossless_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/lossless_msa.c \
    $(PATH_PREFIX)dsp/lossless_neon.$(NEON) \
    $(PATH_PREFIX)dsp/lossless_sse2.c \
    $(PATH_PREFIX)dsp/rescaler.c \
    $(PATH_PREFIX)dsp/rescaler_mips32.c \
    $(PATH_PREFIX)dsp/rescaler_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/rescaler_msa.c \
    $(PATH_PREFIX)dsp/rescaler_neon.$(NEON) \
    $(PATH_PREFIX)dsp/rescaler_sse2.c \
    $(PATH_PREFIX)dsp/upsampling.c \
    $(PATH_PREFIX)dsp/upsampling_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/upsampling_msa.c \
    $(PATH_PREFIX)dsp/upsampling_neon.$(NEON) \
    $(PATH_PREFIX)dsp/upsampling_sse2.c \
    $(PATH_PREFIX)dsp/upsampling_sse41.c \
    $(PATH_PREFIX)dsp/yuv.c \
    $(PATH_PREFIX)dsp/yuv_mips32.c \
    $(PATH_PREFIX)dsp/yuv_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/yuv_neon.$(NEON) \
    $(PATH_PREFIX)dsp/yuv_sse2.c \
    $(PATH_PREFIX)dsp/yuv_sse41.c \

dsp_enc_srcs := \
    $(PATH_PREFIX)dsp/cost.c \
    $(PATH_PREFIX)dsp/cost_mips32.c \
    $(PATH_PREFIX)dsp/cost_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/cost_neon.$(NEON) \
    $(PATH_PREFIX)dsp/cost_sse2.c \
    $(PATH_PREFIX)dsp/enc.c \
    $(PATH_PREFIX)dsp/enc_mips32.c \
    $(PATH_PREFIX)dsp/enc_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/enc_msa.c \
    $(PATH_PREFIX)dsp/enc_neon.$(NEON) \
    $(PATH_PREFIX)dsp/enc_sse2.c \
    $(PATH_PREFIX)dsp/enc_sse41.c \
    $(PATH_PREFIX)dsp/lossless_enc.c \
    $(PATH_PREFIX)dsp/lossless_enc_mips32.c \
    $(PATH_PREFIX)dsp/lossless_enc_mips_dsp_r2.c \
    $(PATH_PREFIX)dsp/lossless_enc_msa.c \
    $(PATH_PREFIX)dsp/lossless_enc_neon.$(NEON) \
    $(PATH_PREFIX)dsp/lossless_enc_sse2.c \
    $(PATH_PREFIX)dsp/lossless_enc_sse41.c \
    $(PATH_PREFIX)dsp/ssim.c \
    $(PATH_PREFIX)dsp/ssim_sse2.c \

enc_srcs := \
    $(PATH_PREFIX)enc/alpha_enc.c \
    $(PATH_PREFIX)enc/analysis_enc.c \
    $(PATH_PREFIX)enc/backward_references_cost_enc.c \
    $(PATH_PREFIX)enc/backward_references_enc.c \
    $(PATH_PREFIX)enc/config_enc.c \
    $(PATH_PREFIX)enc/cost_enc.c \
    $(PATH_PREFIX)enc/filter_enc.c \
    $(PATH_PREFIX)enc/frame_enc.c \
    $(PATH_PREFIX)enc/histogram_enc.c \
    $(PATH_PREFIX)enc/iterator_enc.c \
    $(PATH_PREFIX)enc/near_lossless_enc.c \
    $(PATH_PREFIX)enc/picture_enc.c \
    $(PATH_PREFIX)enc/picture_csp_enc.c \
    $(PATH_PREFIX)enc/picture_psnr_enc.c \
    $(PATH_PREFIX)enc/picture_rescale_enc.c \
    $(PATH_PREFIX)enc/picture_tools_enc.c \
    $(PATH_PREFIX)enc/predictor_enc.c \
    $(PATH_PREFIX)enc/quant_enc.c \
    $(PATH_PREFIX)enc/syntax_enc.c \
    $(PATH_PREFIX)enc/token_enc.c \
    $(PATH_PREFIX)enc/tree_enc.c \
    $(PATH_PREFIX)enc/vp8l_enc.c \
    $(PATH_PREFIX)enc/webp_enc.c \

mux_srcs := \
    $(PATH_PREFIX)mux/anim_encode.c \
    $(PATH_PREFIX)mux/muxedit.c \
    $(PATH_PREFIX)mux/muxinternal.c \
    $(PATH_PREFIX)mux/muxread.c \

utils_dec_srcs := \
    $(PATH_PREFIX)utils/bit_reader_utils.c \
    $(PATH_PREFIX)utils/color_cache_utils.c \
    $(PATH_PREFIX)utils/filters_utils.c \
    $(PATH_PREFIX)utils/huffman_utils.c \
    $(PATH_PREFIX)utils/quant_levels_dec_utils.c \
    $(PATH_PREFIX)utils/random_utils.c \
    $(PATH_PREFIX)utils/rescaler_utils.c \
    $(PATH_PREFIX)utils/thread_utils.c \
    $(PATH_PREFIX)utils/utils.c \

utils_enc_srcs := \
    $(PATH_PREFIX)utils/bit_writer_utils.c \
    $(PATH_PREFIX)utils/huffman_encode_utils.c \
    $(PATH_PREFIX)utils/quant_levels_utils.c \

################################################################################
# libwebpdecoder

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(dec_srcs) \
    $(dsp_dec_srcs) \
    $(utils_dec_srcs) \

LOCAL_CFLAGS := $(WEBP_CFLAGS)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/libwebp

# prefer arm over thumb mode for performance gains
LOCAL_ARM_MODE := arm

ifeq ($(USE_CPUFEATURES),yes)
  LOCAL_STATIC_LIBRARIES := cpufeatures
endif

LOCAL_MODULE := webpdecoder_static

include $(BUILD_STATIC_LIBRARY)

ifeq ($(ENABLE_SHARED),1)
include $(CLEAR_VARS)

LOCAL_WHOLE_STATIC_LIBRARIES := webpdecoder_static

LOCAL_MODULE := webpdecoder

include $(BUILD_SHARED_LIBRARY)
endif  # ENABLE_SHARED=1

################################################################################
# libwebp

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(dsp_enc_srcs) \
    $(enc_srcs) \
    $(utils_enc_srcs) \

LOCAL_CFLAGS := $(WEBP_CFLAGS)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/libwebp

# prefer arm over thumb mode for performance gains
LOCAL_ARM_MODE := arm

LOCAL_WHOLE_STATIC_LIBRARIES := webpdecoder_static

LOCAL_MODULE := webp

ifeq ($(ENABLE_SHARED),1)
  include $(BUILD_SHARED_LIBRARY)
else
  include $(BUILD_STATIC_LIBRARY)
endif

################################################################################
# libwebpdemux

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(demux_srcs)

LOCAL_CFLAGS := $(WEBP_CFLAGS)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/libwebp

# prefer arm over thumb mode for performance gains
LOCAL_ARM_MODE := arm

LOCAL_MODULE := webpdemux

ifeq ($(ENABLE_SHARED),1)
  LOCAL_SHARED_LIBRARIES := webp
  include $(BUILD_SHARED_LIBRARY)
else
  LOCAL_STATIC_LIBRARIES := webp
  include $(BUILD_STATIC_LIBRARY)
endif

################################################################################
# libwebpmux

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(mux_srcs)

LOCAL_CFLAGS := $(WEBP_CFLAGS)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/libwebp

# prefer arm over thumb mode for performance gains
LOCAL_ARM_MODE := arm

LOCAL_MODULE := webpmux

ifeq ($(ENABLE_SHARED),1)
  LOCAL_SHARED_LIBRARIES := webp
  include $(BUILD_SHARED_LIBRARY)
else
  LOCAL_STATIC_LIBRARIES := webp
  include $(BUILD_STATIC_LIBRARY)
endif

################################################################################

ifeq ($(USE_CPUFEATURES),yes)
  $(call import-module,android/cpufeatures)
endif