package com.irwin.webplib;

/**
 * Author: irwin
 * Created At: 2019-10-11
 */
public class AnimWebPMaker {

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

    public AnimWebPMaker() {
        mConfigChanged = true;
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
        return nativeAddImage(data, data.length, duration, quality, lossless) == 0;
    }

    private void checkInit() {
        if (nativeContext == 0) {
            nativeInit();
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
        return make(loopCount, outputPath) == 0;
    }

    public void reset() {
        nativeRelease();
        nativeContext = 0;
        nativeInit();
    }

    public void release() {
        nativeRelease();
    }

    private native void nativeInit();

    private native int nativeSetup();

    private native void config(int min, int max, boolean minSize, boolean mixed, int frameDuration);

    private native int nativeAddImage(byte[] data, int size, int duration, float quality, boolean lossless);

    private native int make(int loopCount, String outputPath);

    private native void nativeRelease();

    private native void nativeFinalize();
}
