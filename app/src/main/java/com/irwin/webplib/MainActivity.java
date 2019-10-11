package com.irwin.webplib;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private WebpProcessor mWebpProcessor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mWebpProcessor = new WebpProcessor();
    }

    @Override
    protected void onStart() {
        super.onStart();
        checkPermission();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    private void checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            }
        }
    }

    public void onClick(final View v) {
        v.setBackgroundColor(Color.RED);
        final String[] array = new String[]{
                new File(Environment.getExternalStorageDirectory(), "frame_2_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_3_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_4_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_5_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_6_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_7_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_8_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_9_delay-0.1s.webp").getAbsolutePath(),
                new File(Environment.getExternalStorageDirectory(), "frame_10_delay-0.1s.webp").getAbsolutePath()
        };
        new Thread(new Runnable() {
            @Override
            public void run() {
                int ret = mWebpProcessor.compose(array, true, 0, 100, 80F, new File(Environment.getExternalStorageDirectory(), "webptest.cnt").getAbsolutePath());
                Log.i("WebPTest", "Compose result: " + ret);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        v.setBackgroundColor(Color.TRANSPARENT);
                    }
                });
            }
        }).start();
    }
}
