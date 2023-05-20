package com.vaHsu.base;

public interface IVideoFrameProvider {
    public int connectToDeliverySink(IVideoFrameConsumer consumer);
    public int disConnectToDeliverySink(IVideoFrameProvider consumer);
}
