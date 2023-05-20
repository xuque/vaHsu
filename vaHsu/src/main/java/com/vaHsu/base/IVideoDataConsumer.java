package com.vaHsu.base;

public interface IVideoDataConsumer {
    public int onDeliveryData(byte[] buffer, int buffLen);
}
