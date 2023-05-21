package com.vaHsu.encoder;

public class VideoEncoderConfig {
    public int mWidth;
    public int mHeight;
    public int mFramerate;
    public int mBitrate;
    public int mGop;

    public VideoEncoderConfig() {

    }

    public VideoEncoderConfig(int width, int height) {
        mWidth = width;
        mHeight = height;
    }

    public void setFramerate(int framerate) {
        mFramerate = framerate;
    }

    public void setGop(int gop) {
        mGop = gop;
    }

    public boolean isSameResolution(int width, int height) {
        return (mWidth == width && mHeight == height);
    }

}
