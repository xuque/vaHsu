package com.vaHsu;


import android.content.Context;

import com.vaHsu.capture.CaptureObserver;
import com.vaHsu.capture.IVideoCapture;
import com.vaHsu.capture.camera.VideoCaptureConfig;
import com.vaHsu.capture.camera.camera1.Camera1Capture;
import com.vaHsu.decoder.IVideoDecoder;
import com.vaHsu.decoder.ffmpeg.FFmpegDecoder;

public class VaHsuApi implements CaptureObserver {
    static {
//        System.loadLibrary("avutil");
//        System.loadLibrary("swresample");
//        System.loadLibrary("avformat");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("swscale");
//        System.loadLibrary("avcodec");
//        System.loadLibrary("vaHsu");
    }

    private VaHsuApi() {}

    private static VaHsuApi mInstance = null;
    public static VaHsuApi instance() {
        if (mInstance == null) {
            synchronized (VaHsuApi.class) {
                if (mInstance == null) {
                    mInstance = new VaHsuApi();
                }
            }
        }
        return mInstance;
    }

    public IVideoDecoder createDecoder(boolean soft, int type) {
        return new FFmpegDecoder();
    }

    public IVideoCapture createCapture(Context context) {
        IVideoCapture capture = new Camera1Capture();
        capture.initialize(context, this);
        return capture;
    }

    @Override
    public void onCameraOpenSuccess() {

    }

    @Override
    public void onCameraStopped() {

    }

    @Override
    public void onCameraError(String error) {

    }

    @Override
    public void onCameraDisconnected() {

    }

    @Override
    public void onCameraClosed() {

    }

    @Override
    public void onFirstFrameAvailable() {

    }
}
