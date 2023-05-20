package com.vaHsu;

import com.vaHsu.decoder.IVideoDecoder;
import com.vaHsu.decoder.ffmpeg.FFmpegDecoder;

public class VaHsuApi {
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

}
