package com.irwin.animwebp;


import android.graphics.Bitmap;
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

    public static boolean makeOnce(String[] images, boolean mixed, int loopCount, int duration, int quality, String outputPath) {
        return (nativeMakeOnce(images, mixed, loopCount, duration, quality, outputPath) != 0);
    }

    public AnimWebPMaker() {
        mConfigChanged = true;
        quality = 100;
        frameDuration = 100;
        mixed = true;
        lossless = false;
    }

    public int getMin() {
        return min;
    }

    public void setMin(int min) {
        this.min = min;
        configChange();
    }

    public int getMax() {
        return max;
    }

    public void setMax(int max) {
        this.max = max;
        configChange();
    }

    public boolean isMinSize() {
        return minSize;
    }

    public void setMinSize(boolean minSize) {
        this.minSize = minSize;
        configChange();
    }

    public boolean isMixed() {
        return mixed;
    }

    public void setMixed(boolean mixed) {
        this.mixed = mixed;
        configChange();
    }

    public int getLoopCount() {
        return loopCount;
    }

    public void setLoopCount(int loopCount) {
        this.loopCount = loopCount;
    }

    public int getFrameDuration() {
        return frameDuration;
    }

    public void setFrameDuration(int frameDuration) {
        this.frameDuration = frameDuration;
        configChange();
    }

    public int getQuality() {
        return quality;
    }

    public void setQuality(int quality) {
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
        ByteArrayOutputStream outs = null;
        try {
            outs = new ByteArrayOutputStream();
            bitmap.compress(Bitmap.CompressFormat.WEBP, 100, outs);
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

    public boolean addImage(byte[] data, int duration, float quality, boolean lossless) {
        checkApplyConfig();
        return nativeAddImage(data, data.length, duration, quality, lossless) != 0;
    }

    private void checkInit() {
        if (nativeContext == 0) {
            Log.i(TAG, "Setup native: " + nativeSetup());
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

    private native void config(int min, int max, boolean minSize, boolean mixed, int frameDuration);

    private native int nativeAddImage(byte[] data, int size, int duration, float quality, boolean lossless);

    private native int make(int loopCount, String outputPath);

    private native void nativeRelease();

    private native void nativeFinalize();

    static {
        System.loadLibrary("anim_maker");
        nativeInit();
    }
}
