package com.irwin.webplib;

/**
 * Author: irwin Created At: 2019-10-10
 */
public class WebpProcessor {

    public native int compose(String[] images,boolean mixed, int loopCount, int duration, float quality, String outputPath);

    static {
        System.loadLibrary("img2webp");
    }
}
