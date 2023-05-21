package com.vaHsu.capture.camera;

import android.support.annotation.Nullable;

public class VideoCaptureConfig {
    private int width;
    private int height;
    private int framerate;

    private boolean faceCamera;
    @Nullable private String cameraId;

    public VideoCaptureConfig() {

    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getFramerate() {
        return framerate;
    }

    public boolean isFaceCamera() {
        return faceCamera;
    }

    @Nullable
    public String getCameraId() {
        return cameraId;
    }
}
