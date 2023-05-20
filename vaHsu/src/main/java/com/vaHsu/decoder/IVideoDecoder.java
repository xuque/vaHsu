package com.vaHsu.decoder;

public interface IVideoDecoder {
    public int init(int width, int height);
    public int deInit();
    public int process(byte[] buffer, int buffLen);
}
