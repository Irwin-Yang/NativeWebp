package com.irwin.webplib;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.irwin.animwebp.AnimWebPMaker;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private AnimWebPMaker mMaker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mMaker = new AnimWebPMaker();
        mMaker.setFrameDuration(100);
        mMaker.setLoopCount(0);
        mMaker.setQuality(80);
        mMaker.setMixed(true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        checkPermission();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMaker.release();
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
                long start = System.currentTimeMillis();
                Log.i("WebPTest", "Compose result: " + AnimWebPMaker.makeOnce(array, true, 0, 100, 80, new File(Environment.getExternalStorageDirectory(), "webptest.cnt").getAbsolutePath()) + "    Cost: " + (System.currentTimeMillis() - start));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        v.setBackgroundColor(Color.TRANSPARENT);
                    }
                });
            }
        }).start();
    }

    public void onClickAsync(final View v) {
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
                mMaker.reset();
                mMaker.setOutputPath(new File(Environment.getExternalStorageDirectory(), "webptest_async.cnt").getAbsolutePath());
                Bitmap bitmap;

                long start = System.currentTimeMillis();
                for (String path : array) {
                    bitmap = BitmapFactory.decodeFile(path);
//                    mMaker.addImage(AnimWebPMaker.bitmap2Array(bitmap));
                    Log.i("WebPTest", "Add frame: " + mMaker.addImage(AnimWebPMaker.bitmap2Array(bitmap)));
                    bitmap.recycle();
                }
                Log.i("WebPTest", "make: " + mMaker.make() + "    Cost: " + (System.currentTimeMillis() - start));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        v.setBackgroundColor(Color.TRANSPARENT);
                    }
                });
            }
        }).start();
    }


    public void onClickPath(final View v) {
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
                mMaker.reset();
                mMaker.setOutputPath(new File(Environment.getExternalStorageDirectory(), "webptest_async_path.cnt").getAbsolutePath());

                long start = System.currentTimeMillis();
                for (String path : array) {
                    Log.i("WebPTest", "Add frame: " + mMaker.addImage(path));
                }
                Log.i("WebPTest", "make: " + mMaker.make() + "    Cost: " + (System.currentTimeMillis() - start));
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
