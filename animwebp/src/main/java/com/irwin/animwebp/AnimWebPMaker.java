package com.irwin.animwebp;


import android.graphics.Bitmap;
import android.support.annotation.IntRange;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Author: irwin
 * Created At: 2019-10-11
 */
public class AnimWebPMaker {
    private static final String TAG = AnimWebPMaker.class.getSimpleName();
    private long nativeContext;
    private int min;
    private int max;
    private boolean minSize;
    private boolean mixed;
    private int loopCount;
    private String outputPath;
    private int frameDuration;
    private int quality;
    private boolean lossless;
    private boolean mConfigChanged;

    public static boolean makeOnce(String[] images, boolean mixed, int loopCount, int duration, @IntRange(from = 1, to = 100) int quality, String outputPath) {
        return (nativeMakeOnce(images, mixed, loopCount, duration, quality, outputPath) != 0);
    }

    public AnimWebPMaker() {
        mConfigChanged = true;
        quality = 100;
        frameDuration = 100;
        mixed = true;
        minSize = false;
        lossless = false;
    }

    public int getMin() {
        return min;
    }

    /**
     * minimum number of frame between key-frames (0=disable key-frames altogether)
     *
     * @param min
     */
    public void setMin(int min) {
        this.min = min;
        configChange();
    }

    public int getMax() {
        return max;
    }

    /**
     * maximum number of frame between key-frames(0=only keyframes)
     *
     * @param max
     */
    public void setMax(int max) {
        this.max = max;
        configChange();
    }

    public boolean isMinSize() {
        return minSize;
    }

    /**
     * Enable minimize size, Will be time cost.
     *
     * @param minSize
     */
    public void setMinSize(boolean minSize) {
        this.minSize = minSize;
        configChange();
    }

    public boolean isMixed() {
        return mixed;
    }

    /**
     * use mixed lossy/lossless automatic mode
     *
     * @param mixed
     */
    public void setMixed(boolean mixed) {
        this.mixed = mixed;
        configChange();
    }

    public int getLoopCount() {
        return loopCount;
    }

    /**
     * loop count (default: 0, = infinite loop)
     *
     * @param loopCount
     */
    public void setLoopCount(int loopCount) {
        this.loopCount = loopCount;
    }

    public int getFrameDuration() {
        return frameDuration;
    }

    /**
     * Frame duration in millis.
     *
     * @param frameDuration
     */
    public void setFrameDuration(int frameDuration) {
        this.frameDuration = frameDuration;
        configChange();
    }

    public int getQuality() {
        return quality;
    }

    /**
     * Set frame quality in 1...100
     *
     * @param quality
     */
    public void setQuality(@IntRange(from = 1, to = 100) int quality) {
        this.quality = quality;
        configChange();
    }

    public boolean isLossless() {
        return lossless;
    }

    public void setLossless(boolean lossless) {
        this.lossless = lossless;
        configChange();
    }

    public String getOutputPath() {
        return outputPath;
    }

    public void setOutputPath(String outputPath) {
        this.outputPath = outputPath;
    }

    public static byte[] bitmap2Array(Bitmap bitmap) {
        return bitmap2Array(bitmap, 100);
    }

    public static byte[] bitmap2Array(Bitmap bitmap, int quality) {
        ByteArrayOutputStream outs = null;
        try {
            outs = new ByteArrayOutputStream();
            bitmap.compress(Bitmap.CompressFormat.WEBP, quality, outs);
            outs.flush();
            return outs.toByteArray();
        } catch (IOException e) {
            return null;
        } finally {
            if (outs != null) {
                try {
                    outs.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public boolean addImage(byte[] data) {
        return addImage(data, frameDuration, quality, lossless);
    }

    public boolean addImage(byte[] data, int duration) {
        return addImage(data, duration, quality, lossless);
    }

    public boolean addImage(byte[] data, boolean lossless) {
        return addImage(data, frameDuration, quality, lossless);
    }

    /**
     * @param data
     * @param duration Frame duration
     * @param quality  Frame quality in 1..100
     * @param lossless use lossless mode ,false default
     * @return
     */
    public boolean addImage(byte[] data, int duration, @IntRange(from = 1, to = 100) int quality, boolean lossless) {
        checkApplyConfig();
        return nativeAddImage(data, data.length, duration, quality, lossless) != 0;
    }

    public boolean addImage(String imagePath) {
        return addImage(imagePath, frameDuration, quality, lossless);
    }

    public boolean addImage(String imagePath, int duration) {
        return addImage(imagePath, duration, quality, lossless);
    }

    public boolean addImage(String imagePath, boolean lossless) {
        return addImage(imagePath, frameDuration, quality, lossless);
    }

    /**
     * @param imagePath Absolute path of image file.
     * @param duration  Frame duration
     * @param quality   Frame quality in 1..100
     * @param lossless  use lossless mode ,false default
     * @return
     */
    public boolean addImage(String imagePath, int duration, @IntRange(from = 1, to = 100) int quality, boolean lossless) {
        checkApplyConfig();
        return nativeAddImagePath(imagePath, duration, quality, lossless) != 0;
    }

    private void checkInit() {
        if (nativeContext == 0) {
            nativeSetup();
        }
    }

    private void configChange() {
        mConfigChanged = true;
    }

    private void checkApplyConfig() {
        if (mConfigChanged) {
            mConfigChanged = false;
            checkInit();
            config(min, max, minSize, mixed, frameDuration);
        }
    }

    public boolean make() {
        return make(loopCount, outputPath) != 0;
    }

    public void reset() {
        release();
        nativeSetup();
    }

    public void release() {
        nativeRelease();
        nativeContext = 0;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        try {
            nativeFinalize();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static native void nativeInit();

    private static native int nativeMakeOnce(String[] images, boolean mixed, int loopCount, int duration, float quality, String outputPath);

    private native int nativeSetup();

    private native void setParam(String key,String value);

    private native void config(int min, int max, boolean minSize, boolean mixed, int frameDuration);

    private native int nativeAddImage(byte[] data, int size, int duration, float quality, boolean lossless);

    private native int nativeAddImagePath(String imagePath, int duration, float quality, boolean lossless);

    private native int make(int loopCount, String outputPath);

    private native void nativeRelease();

    private native void nativeFinalize();

    static {
        System.loadLibrary("anim_webp_maker");
        nativeInit();
    }
}
