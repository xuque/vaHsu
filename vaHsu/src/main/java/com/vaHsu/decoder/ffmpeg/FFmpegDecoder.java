package com.vaHsu.decoder.ffmpeg;

import com.vaHsu.decoder.IVideoDecoder;

public class FFmpegDecoder implements IVideoDecoder {
    @Override
    public int init(int width, int height) {
        return initFFmpegDecoder(width, height);
    }

    @Override
    public int deInit() {
        return deInitFFmpegDecoder();
    }

    @Override
    public int process(byte[] buffer, int buffLen) {
        return processBuffer(buffer, buffLen);
    }

    private native static int initFFmpegDecoder(int width, int height);
    private native static int deInitFFmpegDecoder();
    private native static int processBuffer(byte[] buffer, int buffLen);
}
