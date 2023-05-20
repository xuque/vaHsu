package com.vaHsu.base;

public interface IVideoDataProvider {
    public int connectToDeliverySink(IVideoDataConsumer consumer);
    public int disconnectToDeliverySink(IVideoDataConsumer consumer);
}
